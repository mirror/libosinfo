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

G_DEFINE_TYPE (OsinfoDb, osinfo_db, G_TYPE_OBJECT);

#define OSINFO_DB_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_DB, OsinfoDbPrivate))

struct _OsinfoDbPrivate
{
    OsinfoDeviceList *devices;
    OsinfoHypervisorList *hypervisors;
    OsinfoOsList *oses;
};

static void osinfo_db_finalize (GObject *object);

static void
osinfo_db_finalize (GObject *object)
{
    OsinfoDb *self = OSINFO_DB (object);

    g_object_unref(self->priv->devices);
    g_object_unref(self->priv->hypervisors);
    g_object_unref(self->priv->oses);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_db_parent_class)->finalize (object);
}


/* Init functions */
static void
osinfo_db_class_init (OsinfoDbClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_db_finalize;

    g_type_class_add_private (klass, sizeof (OsinfoDbPrivate));
}


static void
osinfo_db_init (OsinfoDb *self)
{
    OsinfoDbPrivate *priv;
    self->priv = priv = OSINFO_DB_GET_PRIVATE(self);

    self->priv->devices = osinfo_devicelist_new();
    self->priv->hypervisors = osinfo_hypervisorlist_new();
    self->priv->oses = osinfo_oslist_new();
}

/** PUBLIC METHODS */

OsinfoDb *osinfo_db_new(void)
{
    return g_object_new(OSINFO_TYPE_DB, NULL);
}

OsinfoHypervisor *osinfo_db_get_hypervisor(OsinfoDb *self, const gchar *id)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);
    g_return_val_if_fail(id != NULL, NULL);

    return OSINFO_HYPERVISOR(osinfo_list_find_by_id(OSINFO_LIST(self->priv->hypervisors), id));
}

OsinfoDevice *osinfo_db_get_device(OsinfoDb *self, const gchar *id)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);
    g_return_val_if_fail(id != NULL, NULL);

    return OSINFO_DEVICE(osinfo_list_find_by_id(OSINFO_LIST(self->priv->devices), id));
}

OsinfoOs *osinfo_db_get_os(OsinfoDb *self, const gchar *id)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);
    g_return_val_if_fail(id != NULL, NULL);

    return OSINFO_OS(osinfo_list_find_by_id(OSINFO_LIST(self->priv->oses), id));
}


OsinfoOsList *osinfo_db_get_os_list(OsinfoDb *self)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);

    return osinfo_oslist_new_copy(self->priv->oses);
}

OsinfoHypervisorList *osinfo_db_get_hypervisor_list(OsinfoDb *self)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);

    return osinfo_hypervisorlist_new_copy(self->priv->hypervisors);
}

OsinfoDeviceList *osinfo_db_get_device_list(OsinfoDb *self)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);

    return osinfo_devicelist_new_copy(self->priv->devices);
}


void osinfo_db_add_os(OsinfoDb *self, OsinfoOs *os)
{
    g_return_if_fail(OSINFO_IS_DB(self));
    g_return_if_fail(OSINFO_IS_OS(os));

    osinfo_list_add(OSINFO_LIST(self->priv->oses), OSINFO_ENTITY(os));
}


void osinfo_db_add_hypervisor(OsinfoDb *self, OsinfoHypervisor *hv)
{
    g_return_if_fail(OSINFO_IS_DB(self));
    g_return_if_fail(OSINFO_IS_HYPERVISOR(hv));

    osinfo_list_add(OSINFO_LIST(self->priv->hypervisors), OSINFO_ENTITY(hv));
}


void osinfo_db_add_device(OsinfoDb *self, OsinfoDevice *device)
{
    g_return_if_fail(OSINFO_IS_DB(self));
    g_return_if_fail(OSINFO_IS_DEVICE(device));

    osinfo_list_add(OSINFO_LIST(self->priv->devices), OSINFO_ENTITY(device));
}


struct osinfo_db_populate_values_args {
    GHashTable *values;
    const gchar *property;
};

static gboolean osinfo_db_get_property_values_in_entity(OsinfoList *list, OsinfoEntity *entity, gpointer data)
{
    struct osinfo_db_populate_values_args *args = data;
    GHashTable *newValues = args->values;
    const gchar *property = args->property;
    GList *values = osinfo_entity_get_param_value_list(entity, property);
    GList *tmp = values;

    while (tmp) {
        gchar *value = tmp->data;

	if (!g_hash_table_lookup(newValues, value)) {
	    g_hash_table_insert(newValues,
				value,
				GINT_TO_POINTER(1));
	}

	tmp = tmp->next;
    }

    g_list_free(values);

    return FALSE; // Continue iterating
}

static GList *osinfo_db_unique_values_for_property_in_entity(OsinfoList *entities, const gchar *propName)
{
    /* Delibrately no free func for key, since we return those to caller */
    GHashTable *values = g_hash_table_new(g_str_hash, g_str_equal);
    GList *ret;
    struct osinfo_db_populate_values_args args = { values, propName };

    osinfo_list_foreach(entities, osinfo_db_get_property_values_in_entity, &args);

    ret = g_hash_table_get_keys(values);
    g_hash_table_unref(values);
    return ret;
}

// Get me all unique values for property "vendor" among operating systems
GList *osinfo_db_unique_values_for_property_in_os(OsinfoDb *self, const gchar *propName)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);
    g_return_val_if_fail(propName != NULL, NULL);

    return osinfo_db_unique_values_for_property_in_entity(OSINFO_LIST(self->priv->oses), propName);
}

// Get me all unique values for property "vendor" among hypervisors
GList *osinfo_db_unique_values_for_property_in_hv(OsinfoDb *self, const gchar *propName)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);
    g_return_val_if_fail(propName != NULL, NULL);

    return osinfo_db_unique_values_for_property_in_entity(OSINFO_LIST(self->priv->hypervisors), propName);
}

// Get me all unique values for property "vendor" among devices
GList *osinfo_db_unique_values_for_property_in_dev(OsinfoDb *self, const gchar *propName)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);
    g_return_val_if_fail(propName != NULL, NULL);

    return osinfo_db_unique_values_for_property_in_entity(OSINFO_LIST(self->priv->devices), propName);
}

struct __osinfoOsCheckRelationshipArgs {
    OsinfoList *list;
    OsinfoOsRelationship relshp;
};


static gboolean __osinfoAddOsIfRelationship(OsinfoList *list, OsinfoEntity *entity, gpointer data)
{
    struct __osinfoOsCheckRelationshipArgs *args = data;
    OsinfoOs *os = OSINFO_OS(entity);
    OsinfoList *newList = args->list;
    OsinfoOsList *thisList = osinfo_os_get_related(os, args->relshp);
    int i;

    for (i = 0 ; i < osinfo_list_get_length(OSINFO_LIST(thisList)) ; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(OSINFO_LIST(thisList), i);
	osinfo_list_add(newList, entity);
    }

    g_object_unref(thisList);

    return FALSE;
}

// Get me all OSes that 'upgrade' another OS (or whatever relationship is specified)
OsinfoOsList *osinfo_db_unique_values_for_os_relationship(OsinfoDb *self, OsinfoOsRelationship relshp)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);

    // Create list
    OsinfoOsList *newList = osinfo_oslist_new();

    struct __osinfoOsCheckRelationshipArgs args = {OSINFO_LIST (newList), relshp};

    osinfo_list_foreach(OSINFO_LIST(self->priv->oses), __osinfoAddOsIfRelationship, &args);

    return newList;
}
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
