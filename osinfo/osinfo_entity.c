/*
 * libosinfo:
 *
 * Copyright (C) 2009-2012 Red Hat, Inc.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Authors:
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <osinfo/osinfo.h>

G_DEFINE_ABSTRACT_TYPE (OsinfoEntity, osinfo_entity, G_TYPE_OBJECT);

#define OSINFO_ENTITY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_ENTITY, OsinfoEntityPrivate))

/**
 * SECTION:osinfo_entity
 * @short_description: Abstract base class for metadata objects
 * @see_also: #OsinfoList, #OsinfoDb
 *
 * #OsinfoEntity is an abstract base class for all objects against which
 * metadata needs to be recorded. Every object has a unique identifier,
 * which is recommended to be in URI format. Named, multi-valued data
 * parameters can be associated with each entity. When filtering lists
 * of entities, the parameter values can be used for matching.
 */

struct _OsinfoEntityPrivate
{
    gchar *id;

    // Key: gchar*
    // Value: GList of gchar* values
    GHashTable *params;
};

static void osinfo_entity_finalize (GObject *object);

enum {
    PROP_0,

    PROP_ID,
};

static void
osinfo_entity_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    OsinfoEntity *entity = OSINFO_ENTITY (object);

    switch (property_id)
        {
        case PROP_ID:
            g_free(entity->priv->id);
            entity->priv->id = g_value_dup_string (value);
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
        }
}

static void
osinfo_entity_get_property (GObject    *object,
                            guint       property_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
    OsinfoEntity *entity = OSINFO_ENTITY (object);

    switch (property_id)
        {
        case PROP_ID:
            g_value_set_string (value, entity->priv->id);
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
        }
}

static void
osinfo_entity_finalize (GObject *object)
{
    OsinfoEntity *entity = OSINFO_ENTITY (object);

    g_free(entity->priv->id);
    g_hash_table_destroy(entity->priv->params);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_entity_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_entity_class_init (OsinfoEntityClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);
    GParamSpec *pspec;

    g_klass->set_property = osinfo_entity_set_property;
    g_klass->get_property = osinfo_entity_get_property;

    /**
     * OsinfoEntity:id:
     *
     * The unique identifier for the entity The format of identifiers
     * is undefined, but the recommended practice is to use a URI.
     * This parameter must be set at time of construction as no
     * default value is provided.
     */
    pspec = g_param_spec_string ("id",
                                 "ID",
                                 "Unique identifier",
                                 NULL /* default value */,
                                 G_PARAM_CONSTRUCT_ONLY |
                                 G_PARAM_READWRITE |
                                 G_PARAM_STATIC_NAME |
                                 G_PARAM_STATIC_NICK |
                                 G_PARAM_STATIC_BLURB);
    g_object_class_install_property (g_klass,
                                     PROP_ID,
                                     pspec);

    g_klass->finalize = osinfo_entity_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoEntityPrivate));
}

static void osinfo_entity_param_value_free(gpointer value, gpointer opaque G_GNUC_UNUSED)
{
    g_free(value);
}

static void osinfo_entity_param_values_free(gpointer values)
{
    g_list_foreach(values, osinfo_entity_param_value_free, NULL);
    g_list_free(values);
}


static void
osinfo_entity_init (OsinfoEntity *entity)
{
    OsinfoEntityPrivate *priv;
    entity->priv = priv = OSINFO_ENTITY_GET_PRIVATE(entity);

    entity->priv->params = g_hash_table_new_full(g_str_hash,
                                               g_str_equal,
                                               g_free,
                                               osinfo_entity_param_values_free);
}


/**
 * osinfo_entity_set_param:
 * @entity: OsinfoEntity containing the parameters
 * @key: the name of the key
 * @value: the data to associated with that key
 *
 * Sets a new parameter against the entity. If the key already
 * has a value associated with it, the existing value will be
 * cleared.
 */
void osinfo_entity_set_param(OsinfoEntity *entity, const gchar *key, const gchar *value)
{
    g_return_if_fail(OSINFO_IS_ENTITY(entity));
    g_return_if_fail(key != NULL);
    g_return_if_fail(value != NULL);

    GList *values = NULL;

    g_hash_table_remove(entity->priv->params, key);
    values = g_list_append(values, g_strdup(value));
    g_hash_table_insert(entity->priv->params, g_strdup(key), values);
}


/**
 * osinfo_entity_add_param:
 * @entity: OsinfoEntity containing the parameters
 * @key: the name of the key
 * @value: the data to associated with that key
 *
 * Adds a new parameter against the entity. A key can have multiple
 * values associated. Thus repeated calls with the same key will
 * build up a list of possible values.
 */
void osinfo_entity_add_param(OsinfoEntity *entity, const gchar *key, const gchar *value)
{
    g_return_if_fail(OSINFO_IS_ENTITY(entity));
    g_return_if_fail(key != NULL);
    g_return_if_fail(value != NULL);

    // First check if there exists an existing array of entries for this key
    // If not, create a ptrarray of strings for this key and insert into map
    gboolean found;
    gpointer origKey, foundValue;
    GList *values = NULL;

    found = g_hash_table_lookup_extended(entity->priv->params, key, &origKey, &foundValue);
    if (found) {
        g_hash_table_steal(entity->priv->params, key);
        g_free(origKey);
        values = foundValue;
    }

    values = g_list_append(values, g_strdup(value));
    g_hash_table_insert(entity->priv->params, g_strdup(key), values);
}


/**
 * osinfo_entity_clear_param:
 * @entity: OsinfoEntity containing the parameters
 * @key: the name of the key
 *
 * Remove all values associated with a key
 */
void osinfo_entity_clear_param(OsinfoEntity *entity, const gchar *key)
{
    g_hash_table_remove(entity->priv->params, key);
}

/**
 * osinfo_entity_get_id:
 * @entity: a OsinfoEntity 
 *
 * Retrieves the unique key for the entity. The format of identifiers
 * is undefined, but the recommended practice is to use a URI.
 *
 * Returns: (transfer none): the unique key for the entity
 */
const gchar *osinfo_entity_get_id(OsinfoEntity *entity)
{
    g_return_val_if_fail(OSINFO_IS_ENTITY(entity), NULL);

    return entity->priv->id;
}


/**
 * osinfo_entity_get_param_keys:
 * @entity: OsinfoEntity containing the parameters
 *
 * Retrieve all the known parameter keys associated with
 * the entity
 *
 * Returns: (transfer container) (element-type utf8): The list of string parameters
 */
GList *osinfo_entity_get_param_keys(OsinfoEntity *entity)
{
    g_return_val_if_fail(OSINFO_IS_ENTITY(entity), NULL);

    return g_hash_table_get_keys(entity->priv->params);
}


/**
 * osinfo_entity_get_param_value:
 * @entity: OsinfoEntity containing the parameters
 * @key: the name of the key
 *
 * Retrieve the parameter value associated with a named key. If
 * multiple values are stored against the key, only the first
 * value is returned. If no value is associated, NULL is returned
 *
 * Returns: (transfer none): the value associated with the key, or NULL
 */
const gchar *osinfo_entity_get_param_value(OsinfoEntity *entity, const gchar *key)
{
    g_return_val_if_fail(OSINFO_IS_ENTITY(entity), NULL);
    g_return_val_if_fail(key != NULL, NULL);

    GList *values;

    values = g_hash_table_lookup(entity->priv->params, key);

    if (values)
        return values->data;
    return NULL;
}


/**
 * osinfo_entity_get_param_value_list:
 * @entity: OsinfoEntity containing the parameters
 * @key: the name of the key
 *
 * Retrieve all the parameter values associated with a named
 * key. If no values are associated, NULL is returned
 *
 * Returns: (transfer container) (element-type utf8): the values associated with the key
 */
GList *osinfo_entity_get_param_value_list(OsinfoEntity *entity, const gchar *key)
{
    g_return_val_if_fail(OSINFO_IS_ENTITY(entity), NULL);
    g_return_val_if_fail(key != NULL, NULL);

    GList *values = g_hash_table_lookup(entity->priv->params, key);

    return g_list_copy(values);
}


/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
