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

G_DEFINE_TYPE (OsinfoOs, osinfo_os, OSINFO_TYPE_PRODUCT);

#define OSINFO_OS_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_OS, OsinfoOsPrivate))

/**
 * SECTION:osinfo_os
 * @short_description: An operating system
 * @see_also: #OsinfoOs, #OsinfoDeployment
 *
 * #OsinfoOs is an entity representing an operating system.
 * Operating systems have a list of supported devices. 
 * There are relationships amongst operating systems to
 * declare which are newest releases, which are clones
 * and which are derived from a common ancestry.
 */

struct _OsinfoOsPrivate
{
    // Value: List of device_link structs
    GList *deviceLinks;
};

struct _OsinfoOsDeviceLink {
    OsinfoDevice *dev;
    gchar *driver;
};


static void osinfo_os_finalize (GObject *object);

static void osinfo_device_link_free(gpointer data, gpointer opaque G_GNUC_UNUSED)
{
    g_object_unref(OSINFO_DEVICELINK(data));
}


static void
osinfo_os_finalize (GObject *object)
{
    OsinfoOs *os = OSINFO_OS (object);

    g_list_foreach(os->priv->deviceLinks, osinfo_device_link_free, NULL);
    g_list_free(os->priv->deviceLinks);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_os_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_os_class_init (OsinfoOsClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_os_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoOsPrivate));
}


static void
osinfo_os_init (OsinfoOs *os)
{
    OsinfoOsPrivate *priv;
    os->priv = priv = OSINFO_OS_GET_PRIVATE(os);

    os->priv->deviceLinks = NULL;
}


/**
 * osinfo_os_new:
 * @id: a unique identifier
 * 
 * Create a new operating system entity
 *
 * Returns: (transfer full): a new operating system entity
 */
OsinfoOs *osinfo_os_new(const gchar *id)
{
    return g_object_new(OSINFO_TYPE_OS,
                        "id", id,
                        NULL);
}


/**
 * osinfo_os_get_devices:
 * @os: an operating system
 * @filter: (allow-none)(transfer none): an optional device property filter
 *
 * Get all devices matching a given filter
 *
 * Returns: (transfer full): A list of devices
 */
OsinfoDeviceList *osinfo_os_get_devices(OsinfoOs *os, OsinfoFilter *filter)
{
    g_return_val_if_fail(OSINFO_IS_OS(os), NULL);
    g_return_val_if_fail(!filter || OSINFO_IS_FILTER(filter), NULL);

    OsinfoDeviceList *newList = osinfo_devicelist_new();
    GList *tmp = NULL;

    tmp = os->priv->deviceLinks;

    while (tmp) {
        OsinfoDeviceLink *link = OSINFO_DEVICELINK(tmp->data);
        OsinfoDevice *dev = osinfo_devicelink_get_target(link);
        if (!filter || osinfo_filter_matches(filter, OSINFO_ENTITY(dev)))
            osinfo_list_add(OSINFO_LIST(newList), OSINFO_ENTITY(dev));

        tmp = tmp->next;
    }

    return newList;
}



/**
 * osinfo_os_get_device_links:
 * @os: an operating system
 * @filter: (allow-none)(transfer none): an optional device property filter
 *
 * Get all devices matching a given filter. The filter
 * matches against the links, not the devices.
 *
 * Returns: (transfer full): A list of device links
 */
OsinfoDeviceLinkList *osinfo_os_get_device_links(OsinfoOs *os, OsinfoFilter *filter)
{
    g_return_val_if_fail(OSINFO_IS_OS(os), NULL);
    g_return_val_if_fail(!filter || OSINFO_IS_FILTER(filter), NULL);

    OsinfoDeviceLinkList *newList = osinfo_devicelinklist_new();
    GList *tmp = NULL;

    tmp = os->priv->deviceLinks;

    while (tmp) {
        OsinfoDeviceLink *link = OSINFO_DEVICELINK(tmp->data);

        if (!filter || osinfo_filter_matches(filter, OSINFO_ENTITY(link)))
            osinfo_list_add(OSINFO_LIST(newList), OSINFO_ENTITY(link));

        tmp = tmp->next;
    }

    return newList;
}


/**
 * osinfo_os_add_device:
 * @os: an operating system
 * @dev: (transfer none): the device to associate with
 *
 * Associated a device with an operating system.  The
 * returned #OsinfoDeviceLink can be used to record
 * extra metadata against the link
 *
 * Returns: (transfer none): the device association
 */
OsinfoDeviceLink *osinfo_os_add_device(OsinfoOs *os, OsinfoDevice *dev)
{
    g_return_val_if_fail(OSINFO_IS_OS(os), NULL);
    g_return_val_if_fail(OSINFO_IS_DEVICE(dev), NULL);

    OsinfoDeviceLink *link = osinfo_devicelink_new(dev);

    os->priv->deviceLinks = g_list_append(os->priv->deviceLinks, link);

    return link;
}
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
