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

G_DEFINE_TYPE (OsinfoList, osinfo_list, G_TYPE_OBJECT);

#define OSINFO_LIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_LIST, OsinfoListPrivate))

static void osinfo_list_finalize (GObject *object);

struct _OsinfoListPrivate
{
    GPtrArray *array;
};

static void
osinfo_list_finalize (GObject *object)
{
    OsinfoList *self = OSINFO_LIST (object);

    g_ptr_array_free(self->priv->array, TRUE);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_list_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_list_class_init (OsinfoListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_list_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoListPrivate));
}

static void
osinfo_list_init (OsinfoList *self)
{
    OsinfoListPrivate *priv;
    self->priv = priv = OSINFO_LIST_GET_PRIVATE(self);

    self->priv->array = g_ptr_array_new_with_free_func(g_object_unref);
}

gint osinfo_list_get_length(OsinfoList *self)
{
    return self->priv->array->len;
}

OsinfoEntity *osinfo_list_get_nth(OsinfoList *self, gint idx)
{
    return g_ptr_array_index(self->priv->array, idx);
}

OsinfoEntity *osinfo_list_find_by_id(OsinfoList *self, const gchar *id)
{
    int i;
    for (i = 0 ; i < self->priv->array->len ; i++) {
        OsinfoEntity *ent = g_ptr_array_index(self->priv->array, i);
	const gchar *thisid = osinfo_entity_get_id(ent);
	if (g_strcmp0(id, thisid) == 0)
	    return ent;
    }
    return NULL;
}


void osinfo_list_add(OsinfoList *self, OsinfoEntity *entity)
{
    g_object_ref(entity);
    g_ptr_array_add(self->priv->array, entity);
}


void osinfo_list_add_filtered(OsinfoList *self, OsinfoList *source, OsinfoFilter *filter)
{
    int i, len;
    len = osinfo_list_get_length(source);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(source, i);
        if (osinfo_filter_matches(filter, entity))
	    osinfo_list_add(self, entity);
    }
}


void osinfo_list_add_intersection(OsinfoList *self, OsinfoList *sourceOne, OsinfoList *sourceTwo)
{
    int i, len;

    // Make set representation of otherList and newList
    GHashTable *otherSet = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTable *newSet = g_hash_table_new(g_str_hash, g_str_equal);

    // Add all from otherList to otherSet
    len = osinfo_list_get_length(sourceTwo);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(sourceTwo, i);
        g_hash_table_insert(otherSet, osinfo_entity_get_id(entity), entity);
    }

    // If other contains entity, and new list does not, add to new list
    len = osinfo_list_get_length(sourceOne);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(sourceOne, i);

        if (g_hash_table_lookup(otherSet, osinfo_entity_get_id(entity)) &&
            !g_hash_table_lookup(newSet, osinfo_entity_get_id(entity))) {
	    g_hash_table_insert(newSet, osinfo_entity_get_id(entity), entity);
            osinfo_list_add(self, entity);
        }
    }

    g_hash_table_destroy(otherSet);
    g_hash_table_destroy(newSet);
}


void osinfo_list_add_union(OsinfoList *self, OsinfoList *sourceOne, OsinfoList *sourceTwo)
{
    // Make set version of new list
    GHashTable *newSet = g_hash_table_new(g_str_hash, g_str_equal);

    // Add all from other list to new list
    int i, len;
    len = osinfo_list_get_length(sourceTwo);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(sourceTwo, i);
        osinfo_list_add(self, entity);
        g_hash_table_insert(newSet, osinfo_entity_get_id(entity), entity);
    }

    // Add remaining elements from this list to new list
    len = osinfo_list_get_length(sourceOne);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(sourceOne, i);
        // If new list does not contain element, add to new list
        if (!g_hash_table_lookup(newSet, osinfo_entity_get_id(entity))) {
	    osinfo_list_add(self, entity);
            g_hash_table_insert(newSet, osinfo_entity_get_id(entity), entity);
        }
    }

    g_hash_table_unref(newSet);
}


void osinfo_list_foreach(OsinfoList *self, osinfo_list_iterator iter, gpointer data)
{
    int i;
    for (i = 0 ; i < self->priv->array->len ; i++) {
        OsinfoEntity *ent = g_ptr_array_index(self->priv->array, i);
	iter(self, ent, data);
    }
}
