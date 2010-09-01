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

G_DEFINE_ABSTRACT_TYPE (OsinfoList, osinfo_list, G_TYPE_OBJECT);

#define OSINFO_LIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_LIST, OsinfoListPrivate))

/**
 * SECTION:osinfo_list
 * @short_description: Abstract base class for entity lists
 * @see_also: #OsinfoEntity
 *
 * #OsinfoList provides a way to maintain a list of #OsinfoEntity objects.
 *
 */

struct _OsinfoListPrivate
{
    GPtrArray *array;

    GType elementType;
};

enum {
    PROP_O,

    PROP_ELEMENT_TYPE
};


static void
osinfo_list_set_property(GObject      *object,
			 guint         property_id,
			 const GValue *value,
			 GParamSpec   *pspec)
{
    OsinfoList *list = OSINFO_LIST(object);

    switch (property_id) {
    case PROP_ELEMENT_TYPE:
	list->priv->elementType = g_value_get_gtype(value);
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
osinfo_list_get_property(GObject    *object,
			 guint       property_id,
			 GValue     *value,
			 GParamSpec *pspec)
{
    OsinfoList *list = OSINFO_LIST(object);

    switch (property_id) {
    case PROP_ELEMENT_TYPE:
        g_value_set_gtype(value, list->priv->elementType);
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
osinfo_list_finalize (GObject *object)
{
    OsinfoList *list = OSINFO_LIST (object);

    g_ptr_array_free(list->priv->array, TRUE);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_list_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_list_class_init (OsinfoListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);
    GParamSpec *pspec;

    g_klass->set_property = osinfo_list_set_property;
    g_klass->get_property = osinfo_list_get_property;
    g_klass->finalize = osinfo_list_finalize;

    /**
     * OsinfoList:element-type:
     * 
     * The specialization of the list. The list will be
     * restricted to storing #OsinfoEntity objects of
     * the specified type.
     */
    pspec = g_param_spec_gtype("element-type",
			       "Element type",
			       "List element type",
			       OSINFO_TYPE_ENTITY,
			       G_PARAM_CONSTRUCT_ONLY |
			       G_PARAM_READWRITE |
			       G_PARAM_STATIC_NAME |
			       G_PARAM_STATIC_BLURB |
			       G_PARAM_STATIC_NICK);

    g_object_class_install_property(g_klass,
				    PROP_ELEMENT_TYPE,
				    pspec);

    g_type_class_add_private (klass, sizeof (OsinfoListPrivate));
}

static void
osinfo_list_init (OsinfoList *list)
{
    OsinfoListPrivate *priv;
    list->priv = priv = OSINFO_LIST_GET_PRIVATE(list);

    list->priv->array = g_ptr_array_new_with_free_func(g_object_unref);
}

/**
 * osinfo_list_get_element_type:
 * @list: the entity list
 *
 * Retrieves the type of the subclass of #OsinfoEntity 
 * that may be stored in the list
 *
 * Returns: the type of entity stored
 */
GType osinfo_list_get_element_type(OsinfoList *list)
{
    return list->priv->elementType;
}


/**
 * osinfo_list_get_length:
 * @list: the entity list
 *
 * Retrieves the number of elements currently stored
 * in the list
 *
 * Returns: the list length
 */
gint osinfo_list_get_length(OsinfoList *list)
{
    return list->priv->array->len;
}

/**
 * osinfo_list_get_nth:
 * @list: the entity list
 * @idx: the list position to fetch
 *
 * Retrieves the element in the list at position @idx. If
 * @idx is less than zero, or greater than the number of
 * elements in the list, the results are undefined.
 *
 * Returns: (transfer none): the list element
 */
OsinfoEntity *osinfo_list_get_nth(OsinfoList *list, gint idx)
{
    return g_ptr_array_index(list->priv->array, idx);
}


/**
 * osinfo_list_find_by_id:
 * @list: the entity list
 * @id: the unique identifier
 *
 * Search the list looking for the entity with a matching
 * unique identifier.
 *
 * Returns: (transfer none): the matching entity, or NULL
 */
OsinfoEntity *osinfo_list_find_by_id(OsinfoList *list, const gchar *id)
{
    int i;
    for (i = 0 ; i < list->priv->array->len ; i++) {
        OsinfoEntity *ent = g_ptr_array_index(list->priv->array, i);
	const gchar *thisid = osinfo_entity_get_id(ent);
	if (g_strcmp0(id, thisid) == 0)
	    return ent;
    }
    return NULL;
}


/**
 * osinfo_list_add:
 * @list: the entity list
 * @entity: (transfer none): the entity to add to the list
 *
 * Adds a new entity to the list.
 */
void osinfo_list_add(OsinfoList *list, OsinfoEntity *entity)
{
    g_return_if_fail(G_TYPE_CHECK_INSTANCE_TYPE(entity, list->priv->elementType));

    g_object_ref(entity);
    g_ptr_array_add(list->priv->array, entity);
}


/**
 * osinfo_list_add_filtered:
 * @list: the entity list
 * @source: (transfer none): the source for elements
 * @filter: (transfer none): filter to process the source with
 *
 * Adds all entities from @source which are matched by @filter. Using one
 * of the constructors in a subclass is preferrable
 * to this method.
 */
void osinfo_list_add_filtered(OsinfoList *list, OsinfoList *source, OsinfoFilter *filter)
{
    int i, len;
    g_return_if_fail(list->priv->elementType == source->priv->elementType);

    len = osinfo_list_get_length(source);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(source, i);
        if (osinfo_filter_matches(filter, entity))
	    osinfo_list_add(list, entity);
    }
}


/**
 * osinfo_list_add_intersection:
 * @list: the entity list
 * @sourceOne: (transfer none): the first list to add
 * @sourceTwo: (transfer none): the second list to add
 *
 * Computes the intersection between @sourceOne and @sourceTwo and
 * adds the resulting list of entities to the @list. Using one
 * of the constructors in a subclass is preferrable
 * to this method.
 */
void osinfo_list_add_intersection(OsinfoList *list, OsinfoList *sourceOne, OsinfoList *sourceTwo)
{
    int i, len;
    g_return_if_fail(list->priv->elementType == sourceOne->priv->elementType);
    g_return_if_fail(list->priv->elementType == sourceTwo->priv->elementType);

    // Make set representation of otherList and newList
    GHashTable *otherSet = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    GHashTable *newSet = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    // Add all from otherList to otherSet
    len = osinfo_list_get_length(sourceTwo);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(sourceTwo, i);
        g_hash_table_insert(otherSet, g_strdup(osinfo_entity_get_id(entity)), entity);
    }

    // If other contains entity, and new list does not, add to new list
    len = osinfo_list_get_length(sourceOne);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(sourceOne, i);

        if (g_hash_table_lookup(otherSet, osinfo_entity_get_id(entity)) &&
            !g_hash_table_lookup(newSet, osinfo_entity_get_id(entity))) {
	    g_hash_table_insert(newSet, g_strdup(osinfo_entity_get_id(entity)), entity);
            osinfo_list_add(list, entity);
        }
    }

    g_hash_table_destroy(otherSet);
    g_hash_table_destroy(newSet);
}


/**
 * osinfo_list_add_union:
 * @list: the entity list
 * @sourceOne: (transfer none): the first list to add
 * @sourceTwo: (transfer none): the second list to add
 *
 * Computes the union between @sourceOne and @sourceTwo and
 * adds the resulting list of entities to the @list. Using one
 * of the constructors in a subclass is preferrable
 * to this method.
 */
void osinfo_list_add_union(OsinfoList *list, OsinfoList *sourceOne, OsinfoList *sourceTwo)
{
    // Make set version of new list
    GHashTable *newSet;
    g_return_if_fail(list->priv->elementType == sourceOne->priv->elementType);
    g_return_if_fail(list->priv->elementType == sourceTwo->priv->elementType);

    newSet = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    // Add all from other list to new list
    int i, len;
    len = osinfo_list_get_length(sourceTwo);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(sourceTwo, i);
        osinfo_list_add(list, entity);
        g_hash_table_insert(newSet, g_strdup(osinfo_entity_get_id(entity)), entity);
    }

    // Add remaining elements from this list to new list
    len = osinfo_list_get_length(sourceOne);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(sourceOne, i);
        // If new list does not contain element, add to new list
        if (!g_hash_table_lookup(newSet, osinfo_entity_get_id(entity))) {
	    osinfo_list_add(list, entity);
            g_hash_table_insert(newSet, g_strdup(osinfo_entity_get_id(entity)), entity);
        }
    }

    g_hash_table_unref(newSet);
}

/**
 * osinfo_list_add_all:
 * @list: the entity list
 * @source: (transfer none): the list to add
 *
 * Adds all entities from @source to @list. Using one
 * of the constructors in a subclass is preferrable
 * to this method.
 */
void osinfo_list_add_all(OsinfoList *list, OsinfoList *source)
{
    int i, len;
    g_return_if_fail(list->priv->elementType == source->priv->elementType);

    len = osinfo_list_get_length(source);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(source, i);
	osinfo_list_add(list, entity);
    }
}


/**
 * osinfo_list_foreach:
 * @list: the entity list
 * @iter: the iterator callback
 * @data: opaque data value
 *
 * Iterate over all entities in the list, invoking @iter. The
 * iterator callback will be passed the opaque data value @data
 */
void osinfo_list_foreach(OsinfoList *list, osinfo_list_iterator iter, gpointer data)
{
    int i;
    for (i = 0 ; i < list->priv->array->len ; i++) {
        OsinfoEntity *ent = g_ptr_array_index(list->priv->array, i);
	iter(list, ent, data);
    }
}
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
