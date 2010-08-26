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

G_DEFINE_TYPE (OsinfoFilter, osinfo_filter, G_TYPE_OBJECT);

#define OSINFO_FILTER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_FILTER, OsinfoFilterPrivate))

struct _OsinfoFilterPrivate
{
    // Key: Constraint name
    // Value: GList of constraint values
    GHashTable *propertyConstraints;
};


static void osinfo_filter_finalize (GObject *object);
static gboolean osinfo_filter_matches_default(OsinfoFilter *self, OsinfoEntity *entity);

static void
osinfo_filter_finalize (GObject *object)
{
    OsinfoFilter *self = OSINFO_FILTER (object);

    g_hash_table_unref(self->priv->propertyConstraints);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_filter_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_filter_class_init (OsinfoFilterClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_filter_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoFilterPrivate));

    klass->matches = osinfo_filter_matches_default;
}


OsinfoFilter *osinfo_filter_new(void)
{
    return g_object_new(OSINFO_TYPE_FILTER, NULL);
}


static void
osinfo_filter_prop_constraint_free(gpointer value, gpointer opaque G_GNUC_UNUSED)
{
    g_free(value);
}

static void
osinfo_filter_prop_constraints_free(gpointer props)
{
    g_list_foreach(props, osinfo_filter_prop_constraint_free, NULL);
    g_list_free(props);
}


static void
osinfo_filter_init (OsinfoFilter *self)
{
    OsinfoFilterPrivate *priv;
    priv = OSINFO_FILTER_GET_PRIVATE(self);
    self->priv = priv;

    self->priv->propertyConstraints =
        g_hash_table_new_full(g_str_hash,
			      g_str_equal,
			      g_free,
			      osinfo_filter_prop_constraints_free);
}


gint osinfo_filter_add_constraint(OsinfoFilter *self, gchar *propName, gchar *propVal)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(self), -1);
    g_return_val_if_fail(propName != NULL, -1);
    g_return_val_if_fail(propVal != NULL, -1);

    // First check if there exists an array of entries for this key
    // If not, create a ptrarray of strings for this key and insert into map
    gboolean found;
    gpointer origKey, foundValue;
    GList *values = NULL;

    found = g_hash_table_lookup_extended(self->priv->propertyConstraints, propName, &origKey, &foundValue);
    if (found) {
        g_hash_table_steal(self->priv->propertyConstraints, propName);
	g_free(origKey);
        values = foundValue;
    }
    values = g_list_prepend(values, g_strdup(propVal));
    g_hash_table_insert(self->priv->propertyConstraints, g_strdup(propName), values);

    return 0;
}

void osinfo_filter_clear_constraint(OsinfoFilter *self, gchar *propName)
{
    g_hash_table_remove(self->priv->propertyConstraints, propName);
}

void osinfo_filter_clear_constraints(OsinfoFilter *self)
{
    g_hash_table_remove_all(self->priv->propertyConstraints);
}

// get keyset for constraints map
GList *osinfo_filter_get_constraint_keys(OsinfoFilter *self)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(self), NULL);

    return g_hash_table_get_keys(self->priv->propertyConstraints);
}

// get values for given key
GList *osinfo_filter_get_constraint_values(OsinfoFilter *self, gchar *propName)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(self), NULL);
    g_return_val_if_fail(propName != NULL, NULL);

    return g_hash_table_lookup(self->priv->propertyConstraints, propName);
}


struct osinfo_filter_match_args {
    OsinfoFilter *self;
    OsinfoEntity *entity;
    gboolean matched;
};

static void osinfo_filter_match_iterator(gpointer key, gpointer value, gpointer data)
{
    struct osinfo_filter_match_args *args = data;
    OsinfoEntity *entity = args->entity;
    const gchar *propName = key;
    GList *propValues = value;

    GList *values = osinfo_entity_get_param_value_list(entity, propName);

    if (propValues && !values) {
        args->matched = FALSE;
        return;
    }

    while (propValues) {
        const gchar *propValue = propValues->data;
	gboolean found = FALSE;
	GList *tmp = values;
	while (tmp) {
	    const gchar *testValue = tmp->data;
            if (g_strcmp0(propValue, testValue) == 0) {
                found = TRUE;
                break;
            }

	    tmp = tmp->next;
        }
        if (!found) {
	    args->matched = FALSE;
	    return;
	}

        propValues = propValues->next;
    }
}


static gboolean osinfo_filter_matches_default(OsinfoFilter *self, OsinfoEntity *entity)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(self), FALSE);
    g_return_val_if_fail(OSINFO_IS_ENTITY(entity), FALSE);

    struct osinfo_filter_match_args args = { self, entity, TRUE };
    g_hash_table_foreach(self->priv->propertyConstraints,
			 osinfo_filter_match_iterator,
			 &args);

    return args.matched;
}

gboolean osinfo_filter_matches(OsinfoFilter *self, OsinfoEntity *entity)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(self), FALSE);
    g_return_val_if_fail(OSINFO_IS_ENTITY(entity), FALSE);

    return OSINFO_FILTER_GET_CLASS(self)->matches(self, entity);
}
