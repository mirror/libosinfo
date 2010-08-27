/*
 * libosinfo:
 *
 * Copyright (C) 2009-2010 Red Hat, Inc
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 * Authors:
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoOsfilter, osinfo_osfilter, OSINFO_TYPE_FILTER);

#define OSINFO_OSFILTER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_OSFILTER, OsinfoOsfilterPrivate))

struct _OsinfoOsfilterPrivate
{
    // Key: relationship type
    // Value: GList of OsinfoOs *
    // Note: Only used when osfiltering OsinfoOs objects
    GHashTable *osConstraints;
};


static void osinfo_osfilter_finalize (GObject *object);
static gboolean osinfo_osfilter_matches_default(OsinfoFilter *self, OsinfoEntity *entity);

static void
osinfo_osfilter_finalize (GObject *object)
{
    OsinfoOsfilter *self = OSINFO_OSFILTER (object);

    g_hash_table_unref(self->priv->osConstraints);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_osfilter_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_osfilter_class_init (OsinfoOsfilterClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS(klass);
    OsinfoFilterClass *filter_klass = OSINFO_FILTER_CLASS(klass);

    g_klass->finalize = osinfo_osfilter_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoOsfilterPrivate));

    filter_klass->matches = osinfo_osfilter_matches_default;
}


OsinfoOsfilter *osinfo_osfilter_new(void)
{
    return g_object_new(OSINFO_TYPE_OSFILTER, NULL);
}


static void
osinfo_osfilter_os_constraint_free(gpointer value, gpointer opaque G_GNUC_UNUSED)
{
    g_object_unref(value);
}

static void
osinfo_osfilter_os_constraints_free(gpointer relshps)
{
    g_list_foreach(relshps, osinfo_osfilter_os_constraint_free, NULL);
    g_list_free(relshps);
}

static void
osinfo_osfilter_init (OsinfoOsfilter *self)
{
    OsinfoOsfilterPrivate *priv;
    priv = OSINFO_OSFILTER_GET_PRIVATE(self);
    self->priv = priv;

    self->priv->osConstraints =
        g_hash_table_new_full(g_direct_hash,
			      g_direct_equal,
			      NULL,
			      osinfo_osfilter_os_constraints_free);
}


// Only applicable to OSes, ignored by other types of objects
gint osinfo_osfilter_add_os_constraint(OsinfoOsfilter *self, OsinfoOsRelationship relshp, OsinfoOs *os)
{
    g_return_val_if_fail(OSINFO_IS_OSFILTER(self), -1);
    g_return_val_if_fail(OSINFO_IS_OS(os), -1);

    // First check if there exists an array of entries for this key
    // If not, create a ptrarray of strings for this key and insert into map
    gboolean found;
    gpointer origKey, foundValue;
    GList *values = NULL;

    found = g_hash_table_lookup_extended(self->priv->osConstraints, GINT_TO_POINTER(relshp), &origKey, &foundValue);
    if (found) {
        values = foundValue;
        g_hash_table_steal(self->priv->osConstraints, GINT_TO_POINTER(relshp));
    }
    g_object_ref(os);
    values = g_list_prepend(values, os);
    g_hash_table_insert(self->priv->osConstraints, GINT_TO_POINTER(relshp), values);

    return 0;
}

void osinfo_osfilter_clear_os_constraint(OsinfoOsfilter *self, OsinfoOsRelationship relshp)
{
    g_hash_table_remove(self->priv->osConstraints, (gpointer) relshp);
}

void osinfo_osfilter_clear_os_constraints(OsinfoOsfilter *self)
{
    g_hash_table_remove_all(self->priv->osConstraints);
}


// get oses for given relshp
GList *osinfo_osfilter_get_os_constraint_values(OsinfoOsfilter *self, OsinfoOsRelationship relshp)
{
    g_return_val_if_fail(OSINFO_IS_OSFILTER(self), NULL);

    GList *values = g_hash_table_lookup(self->priv->osConstraints, GINT_TO_POINTER(relshp));

    return g_list_copy(values);
}


struct osinfo_osfilter_match_args {
    OsinfoOsfilter *self;
    OsinfoEntity *entity;
    gboolean matched;
};


static void osinfo_osfilter_match_os_iterator(gpointer key, gpointer value, gpointer data)
{
    struct osinfo_osfilter_match_args *args = data;
    OsinfoOs *os = OSINFO_OS(args->entity);
    OsinfoOsRelationship relshp = GPOINTER_TO_INT(key);
    GList *relOses = value;
    OsinfoOsList *oslist = osinfo_os_get_related(os, relshp);
    gboolean ret = TRUE;

    if (relOses && osinfo_list_get_length(OSINFO_LIST(oslist)) == 0) {
        ret = FALSE;
	goto cleanup;
    }

    while (relOses) {
        OsinfoOs *currOs = relOses->data;
        int i;
	gboolean found = FALSE;
	for (i = 0 ; i < osinfo_list_get_length(OSINFO_LIST(oslist)) ; i++) {
	    OsinfoOs *testOs = OSINFO_OS(osinfo_list_get_nth(OSINFO_LIST(oslist), i));
            if (testOs == currOs) {
                found = TRUE;
                break;
            }
        }
        if (!found) {
	    ret = FALSE;
	    goto cleanup;
	}

	relOses = relOses->next;
    }

 cleanup:
    g_object_unref(oslist);
    args->matched = ret;
}

static gboolean osinfo_osfilter_matches_default(OsinfoFilter *filter, OsinfoEntity *entity)
{
    g_return_val_if_fail(OSINFO_IS_OSFILTER(filter), FALSE);
    g_return_val_if_fail(OSINFO_IS_OS(entity), FALSE);
    OsinfoOsfilter *self = OSINFO_OSFILTER(filter);
    struct osinfo_osfilter_match_args args = { self, entity, TRUE };

    if (!OSINFO_FILTER_CLASS (osinfo_osfilter_parent_class)->matches(filter, entity))
        return FALSE;

    g_hash_table_foreach(self->priv->osConstraints,
			 osinfo_osfilter_match_os_iterator,
			 &args);

    return args.matched;
}

