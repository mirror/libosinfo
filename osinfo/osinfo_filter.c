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
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <config.h>

#include <osinfo/osinfo.h>
#include <glib/gi18n-lib.h>

G_DEFINE_TYPE (OsinfoFilter, osinfo_filter, G_TYPE_OBJECT);

#define OSINFO_FILTER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_FILTER, OsinfoFilterPrivate))

/**
 * SECTION:osinfo_filter
 * @short_description: An entity filter
 * @see_also: #OsinfoEntity
 *
 * #OsinfoFilter provides a way to filter OsinfoEntity
 * instances based on their parameter values.
 */

struct _OsinfoFilterPrivate
{
    // Key: Constraint name
    // Value: GList of constraint values
    GHashTable *propertyConstraints;
};


static void osinfo_filter_finalize (GObject *object);
static gboolean osinfo_filter_matches_default(OsinfoFilter *filter, OsinfoEntity *entity);

static void
osinfo_filter_finalize (GObject *object)
{
    OsinfoFilter *filter = OSINFO_FILTER (object);

    g_hash_table_unref(filter->priv->propertyConstraints);

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


/**
 * osinfo_filter_new:
 *
 * Construct a new filter that matches all entities
 *
 * Returns: (transfer full): a filter object
 */
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
osinfo_filter_init (OsinfoFilter *filter)
{
    filter->priv = OSINFO_FILTER_GET_PRIVATE(filter);

    filter->priv->propertyConstraints =
        g_hash_table_new_full(g_str_hash,
                              g_str_equal,
                              g_free,
                              osinfo_filter_prop_constraints_free);
}


/**
 * osinfo_filter_add_constraint:
 * @filter: a filter object
 * @propName: the name of the parameter key
 * @propVal: the required property value
 *
 * Adds a constraint that requires the entity to have
 * a property key @propName with a value of @propVal.
 * If multiple constraints are added for the same
 * @propName, with different values, the entity have
 * all property values.
 */
void osinfo_filter_add_constraint(OsinfoFilter *filter,
                                  const gchar *propName,
                                  const gchar *propVal)
{
    g_return_if_fail(OSINFO_IS_FILTER(filter));
    g_return_if_fail(propName != NULL);
    g_return_if_fail(propVal != NULL);

    // First check if there exists an array of entries for this key
    // If not, create a ptrarray of strings for this key and insert into map
    gboolean found;
    gpointer origKey, foundValue;
    GList *values = NULL;

    found = g_hash_table_lookup_extended(filter->priv->propertyConstraints, propName, &origKey, &foundValue);
    if (found) {
        g_hash_table_steal(filter->priv->propertyConstraints, propName);
        g_free(origKey);
        values = foundValue;
    }
    values = g_list_prepend(values, g_strdup(propVal));
    g_hash_table_insert(filter->priv->propertyConstraints, g_strdup(propName), values);
}


/**
 * osinfo_filter_clear_constraint:
 * @filter: a filter object
 * @propName: name of the key to remove constraints for
 *
 * Remove all filter constraints for the matching property
 * name.
 */
void osinfo_filter_clear_constraint(OsinfoFilter *filter, const gchar *propName)
{
    g_hash_table_remove(filter->priv->propertyConstraints, propName);
}


/**
 * osinfo_filter_clear_constraints:
 * @filter: a filter object
 *
 * Remove all filter property constraints
 */
void osinfo_filter_clear_constraints(OsinfoFilter *filter)
{
    g_hash_table_remove_all(filter->priv->propertyConstraints);
}

/**
 * osinfo_filter_get_constraint_keys:
 * @filter: a filter object
 *
 * Get a list of all constraint property keys
 *
 * Returns: (transfer container)(element-type utf8): List of constraint keys
 */
GList *osinfo_filter_get_constraint_keys(OsinfoFilter *filter)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(filter), NULL);

    return g_hash_table_get_keys(filter->priv->propertyConstraints);
}

/**
 * osinfo_filter_get_constraint_values:
 * @filter: a filter object
 * @propName: the name of the key
 *
 * Get a list values for filter constriants with the named key
 *
 * Returns: (transfer container)(element-type utf8): List of constraint values
 */
GList *osinfo_filter_get_constraint_values(OsinfoFilter *filter, const gchar *propName)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(filter), NULL);
    g_return_val_if_fail(propName != NULL, NULL);

    GList *values = g_hash_table_lookup(filter->priv->propertyConstraints, propName);

    return g_list_copy(values);
}


struct osinfo_filter_match_args {
    OsinfoFilter *filter;
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
            g_list_free(values);
            return;
        }

        propValues = propValues->next;
    }
    g_list_free(values);
}


static gboolean osinfo_filter_matches_default(OsinfoFilter *filter, OsinfoEntity *entity)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(filter), FALSE);
    g_return_val_if_fail(OSINFO_IS_ENTITY(entity), FALSE);

    struct osinfo_filter_match_args args = { filter, entity, TRUE };
    g_hash_table_foreach(filter->priv->propertyConstraints,
                         osinfo_filter_match_iterator,
                         &args);

    return args.matched;
}


/**
 * osinfo_filter_matches:
 * @filter: a filter object
 * @entity: an entity to query
 *
 * Determine of an entity matches a filter
 *
 * Returns: TRUE if entity passes the filter, FALSE otherwise
 */
gboolean osinfo_filter_matches(OsinfoFilter *filter, OsinfoEntity *entity)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(filter), FALSE);
    g_return_val_if_fail(OSINFO_IS_ENTITY(entity), FALSE);

    return OSINFO_FILTER_GET_CLASS(filter)->matches(filter, entity);
}
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
