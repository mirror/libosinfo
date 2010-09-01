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

G_DEFINE_TYPE (OsinfoHypervisor, osinfo_hypervisor, OSINFO_TYPE_ENTITY);

#define OSINFO_HYPERVISOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_HYPERVISOR, OsinfoHypervisorPrivate))

/**
 * SECTION:osinfo_hypervisor
 * @short_description: An operating system
 * @see_also: #OsinfoOs, #OsinfoHypervisor
 *
 * #OsinfoHypervisor is an entity representing an virtualization
 * hypervisor. Hypervisors have a list of supported devices
 */

struct _OsinfoHypervisorPrivate
{
    // Value: List of device_link structs
    GList *deviceLinks;
};

struct _OsinfoHypervisorDeviceLink {
    OsinfoDevice *dev;
    gchar *driver;
};

static void osinfo_hypervisor_finalize (GObject *object);

static void osinfo_device_link_free(gpointer data, gpointer opaque G_GNUC_UNUSED)
{
    struct _OsinfoHypervisorDeviceLink *link = data;
    g_object_unref(link->dev);
    g_free(link->driver);
    g_free(link);
}

static void
osinfo_hypervisor_finalize (GObject *object)
{
    OsinfoHypervisor *hv = OSINFO_HYPERVISOR (object);

    g_list_foreach(hv->priv->deviceLinks, osinfo_device_link_free, NULL);
    g_list_free(hv->priv->deviceLinks);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_hypervisor_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_hypervisor_class_init (OsinfoHypervisorClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_hypervisor_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoHypervisorPrivate));
}

static void
osinfo_hypervisor_init (OsinfoHypervisor *hv)
{
    OsinfoHypervisorPrivate *priv;
    hv->priv = priv = OSINFO_HYPERVISOR_GET_PRIVATE(hv);

    hv->priv->deviceLinks = NULL;
}


/**
 * osinfo_hypervisor_new:
 * @id: a unique identifier
 *
 * Create a new hypervisor entity
 *
 * Returns: (transfer full): A hypervisor entity
 */
OsinfoHypervisor *osinfo_hypervisor_new(const gchar *id)
{
    return g_object_new(OSINFO_TYPE_HYPERVISOR,
			"id", id,
			NULL);
}


/**
 * osinfo_hypervisor_get_devices:
 * @hv: a hypervisor entity
 * @filter: (transfer none)(allow-none): an optional filter
 *
 * Returns: (transfer full): a list of #OsinfoDevice entities
 */
OsinfoDeviceList *osinfo_hypervisor_get_devices(OsinfoHypervisor *hv, OsinfoFilter *filter)
{
    g_return_val_if_fail(OSINFO_IS_HYPERVISOR(hv), NULL);
    g_return_val_if_fail(!filter || OSINFO_IS_FILTER(filter), NULL);

    OsinfoDeviceList *newList = osinfo_devicelist_new();
    GList *tmp = hv->priv->deviceLinks;

    while (tmp) {
        struct _OsinfoHypervisorDeviceLink *link = tmp->data;

        if (!filter || osinfo_filter_matches(filter, OSINFO_ENTITY(link->dev)))
	    osinfo_list_add(OSINFO_LIST(newList), OSINFO_ENTITY(link->dev));

	tmp = tmp->next;
    }

    return newList;
}


/**
 * osinfo_hypervisor_add_device:
 * @hv: a hypervisor entity
 * @dev: (transfer none): the device to associated
 * @driver: the hypervisor driver name
 *
 * Associate a device with a hypervisor
 */
void osinfo_hypervisor_add_device(OsinfoHypervisor *hv, OsinfoDevice *dev, const gchar *driver)
{
    g_return_if_fail(OSINFO_IS_HYPERVISOR(hv));
    g_return_if_fail(OSINFO_IS_DEVICE(dev));
    g_return_if_fail(driver != NULL);

    struct _OsinfoHypervisorDeviceLink *link = g_new0(struct _OsinfoHypervisorDeviceLink, 1);

    g_object_ref(dev);
    link->dev = dev;
    link->driver = g_strdup(driver);

    hv->priv->deviceLinks = g_list_prepend(hv->priv->deviceLinks, link);
}
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
