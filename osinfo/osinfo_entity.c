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

G_DEFINE_ABSTRACT_TYPE (OsinfoEntity, osinfo_entity, G_TYPE_OBJECT);

#define OSINFO_ENTITY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_ENTITY, OsinfoEntityPrivate))

struct _OsinfoEntityPrivate
{
    gchar *id;

    // Key: gchar*
    // Value: GList of gchar* values
    GHashTable *params;
};

static void osinfo_entity_finalize (GObject *object);

enum OSI_ENTITY_PROPERTIES {
    OSI_ENTITY_PROP_0,

    OSI_ENTITY_ID,
};

static void
osinfo_entity_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    OsinfoEntity *self = OSINFO_ENTITY (object);

    switch (property_id)
      {
      case OSI_ENTITY_ID:
        g_free(self->priv->id);
        self->priv->id = g_value_dup_string (value);
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
    OsinfoEntity *self = OSINFO_ENTITY (object);

    switch (property_id)
      {
      case OSI_ENTITY_ID:
        g_value_set_string (value, self->priv->id);
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
    OsinfoEntity *self = OSINFO_ENTITY (object);

    g_free(self->priv->id);
    g_hash_table_destroy(self->priv->params);

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

    pspec = g_param_spec_string ("id",
                                 "ID",
                                 "Contains unique identifier for entity.",
                                 NULL /* default value */,
                                 G_PARAM_CONSTRUCT_ONLY |
				 G_PARAM_READWRITE |
				 G_PARAM_STATIC_NAME |
				 G_PARAM_STATIC_NICK |
				 G_PARAM_STATIC_BLURB);
    g_object_class_install_property (g_klass,
                                     OSI_ENTITY_ID,
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
osinfo_entity_init (OsinfoEntity *self)
{
    OsinfoEntityPrivate *priv;
    self->priv = priv = OSINFO_ENTITY_GET_PRIVATE(self);

    self->priv->params = g_hash_table_new_full(g_str_hash,
					       g_str_equal,
					       g_free,
					       osinfo_entity_param_values_free);
}

void osinfo_entity_add_param(OsinfoEntity *self, const gchar *key, const gchar *value)
{
    g_return_if_fail(OSINFO_IS_ENTITY(self));
    g_return_if_fail(key != NULL);
    g_return_if_fail(value != NULL);

    // First check if there exists an existing array of entries for this key
    // If not, create a ptrarray of strings for this key and insert into map
    gboolean found;
    gpointer origKey, foundValue;
    GList *values = NULL;

    found = g_hash_table_lookup_extended(self->priv->params, key, &origKey, &foundValue);
    if (found) {
        g_hash_table_steal(self->priv->params, key);
	g_free(origKey);
	values = foundValue;
    }

    values = g_list_append(values, g_strdup(value));
    g_hash_table_insert(self->priv->params, g_strdup(key), values);
}

void osinfo_entity_clear_param(OsinfoEntity *self, const gchar *key)
{
    g_hash_table_remove(self->priv->params, key);
}

const gchar *osinfo_entity_get_id(OsinfoEntity *self)
{
    g_return_val_if_fail(OSINFO_IS_ENTITY(self), NULL);

    return self->priv->id;
}

GList *osinfo_entity_get_param_keys(OsinfoEntity *self)
{
    g_return_val_if_fail(OSINFO_IS_ENTITY(self), NULL);

    return g_hash_table_get_keys(self->priv->params);
}

const gchar *osinfo_entity_get_param_value(OsinfoEntity *self, const gchar *key)
{
    g_return_val_if_fail(OSINFO_IS_ENTITY(self), NULL);
    g_return_val_if_fail(key != NULL, NULL);

    GList *values;

    values = g_hash_table_lookup(self->priv->params, key);

    if (values)
        return values->data;
    return NULL;
}

GList *osinfo_entity_get_param_value_list(OsinfoEntity *self, const gchar *key)
{
    g_return_val_if_fail(OSINFO_IS_ENTITY(self), NULL);
    g_return_val_if_fail(key != NULL, NULL);

    GList *values = g_hash_table_lookup(self->priv->params, key);

    return g_list_copy(values);
}


