/*
 * libosinfo:
 *
 * Copyright (C) 2009-2012, 2014 Red Hat, Inc.
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
#include "osinfo/osinfo_product_private.h"
#include <glib/gi18n-lib.h>

G_DEFINE_TYPE(OsinfoPlatform, osinfo_platform, OSINFO_TYPE_PRODUCT);

#define OSINFO_PLATFORM_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), OSINFO_TYPE_PLATFORM, OsinfoPlatformPrivate))

/**
 * SECTION:osinfo_platform
 * @short_description: An virtualization platform
 * @see_also: #OsinfoOs, #OsinfoPlatform
 *
 * #OsinfoPlatform is an entity representing an virtualization
 * platform. Platforms have a list of supported devices
 */

struct _OsinfoPlatformPrivate
{
    // Value: List of device_link structs
    GList *deviceLinks;
};

struct _OsinfoPlatformDeviceLink {
    OsinfoDevice *dev;
    gchar *driver;
};

static void osinfo_device_link_free(gpointer data, gpointer opaque G_GNUC_UNUSED)
{
    g_object_unref(OSINFO_DEVICELINK(data));
}

static void
osinfo_platform_finalize(GObject *object)
{
    OsinfoPlatform *platform = OSINFO_PLATFORM(object);

    g_list_foreach(platform->priv->deviceLinks, osinfo_device_link_free, NULL);
    g_list_free(platform->priv->deviceLinks);

    /* Chain up to the parent class */
    G_OBJECT_CLASS(osinfo_platform_parent_class)->finalize(object);
}

/* Init functions */
static void
osinfo_platform_class_init(OsinfoPlatformClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS(klass);

    g_klass->finalize = osinfo_platform_finalize;
    g_type_class_add_private(klass, sizeof(OsinfoPlatformPrivate));
}

static void
osinfo_platform_init(OsinfoPlatform *platform)
{
    platform->priv = OSINFO_PLATFORM_GET_PRIVATE(platform);
    platform->priv->deviceLinks = NULL;
}


/**
 * osinfo_platform_new:
 * @id: a unique identifier
 *
 * Create a new platform entity
 *
 * Returns: (transfer full): A platform entity
 */
OsinfoPlatform *osinfo_platform_new(const gchar *id)
{
    return g_object_new(OSINFO_TYPE_PLATFORM,
                        "id", id,
                        NULL);
}

struct GetAllDevicesData {
    OsinfoFilter *filter;
    OsinfoDeviceList *devices;
};

static void get_all_devices_cb(OsinfoProduct *product, gpointer user_data)
{
    OsinfoDeviceList *devices;
    OsinfoList *tmp_list;
    struct GetAllDevicesData *foreach_data = (struct GetAllDevicesData *)user_data;

    g_return_if_fail(OSINFO_IS_PLATFORM(product));

    devices = osinfo_platform_get_devices(OSINFO_PLATFORM(product),
                                          foreach_data->filter);
    tmp_list = osinfo_list_new_union(OSINFO_LIST(foreach_data->devices),
                                     OSINFO_LIST(devices));
    g_object_unref(foreach_data->devices);
    g_object_unref(devices);
    foreach_data->devices = OSINFO_DEVICELIST(tmp_list);
}


/**
 * osinfo_platform_get_all_devices:
 * @platform: a platform
 * @filter: (allow-none)(transfer none): an optional device property filter
 *
 * Get all platforms matching a given filter but unlike
 * osinfo_platform_get_devices this function also retrieves devices from
 * all derived and upgraded platforms.
 *
 * Returns: (transfer full): A list of devices
 */
OsinfoDeviceList *osinfo_platform_get_all_devices(OsinfoPlatform *platform,
                                                  OsinfoFilter *filter)
{
    struct GetAllDevicesData foreach_data = {
        .filter = filter,
        .devices = osinfo_devicelist_new()
    };

    osinfo_product_foreach_related(OSINFO_PRODUCT(platform),
                                   OSINFO_PRODUCT_FOREACH_FLAG_UPGRADES |
                                   OSINFO_PRODUCT_FOREACH_FLAG_DERIVES_FROM,
                                   get_all_devices_cb,
                                   &foreach_data);

    return foreach_data.devices;
}

/**
 * osinfo_platform_get_devices:
 * @platform: a platform entity
 * @filter: (transfer none)(allow-none): an optional filter
 *
 * Retrieve all the associated devices matching the filter.
 * The filter matches against the device, not the link.
 *
 * Returns: (transfer full): a list of #OsinfoDevice entities
 */
OsinfoDeviceList *osinfo_platform_get_devices(OsinfoPlatform *platform, OsinfoFilter *filter)
{
    g_return_val_if_fail(OSINFO_IS_PLATFORM(platform), NULL);
    g_return_val_if_fail(!filter || OSINFO_IS_FILTER(filter), NULL);

    OsinfoDeviceList *newList = osinfo_devicelist_new();
    GList *tmp = platform->priv->deviceLinks;

    while (tmp) {
        OsinfoDeviceLink *devlink = OSINFO_DEVICELINK(tmp->data);
        OsinfoDevice *dev = osinfo_devicelink_get_target(devlink);
        if (!filter || osinfo_filter_matches(filter, OSINFO_ENTITY(dev)))
            osinfo_list_add(OSINFO_LIST(newList), OSINFO_ENTITY(dev));

        tmp = tmp->next;
    }

    return newList;
}


/**
 * osinfo_platform_get_device_links:
 * @platform: a platform entity
 * @filter: (transfer none)(allow-none): an optional filter
 *
 * Retrieve all the associated devices matching the filter.
 * The filter matches against the link, not the device.
 *
 * Returns: (transfer full): a list of #OsinfoDevice entities
 */
OsinfoDeviceLinkList *osinfo_platform_get_device_links(OsinfoPlatform *platform, OsinfoFilter *filter)
{
    g_return_val_if_fail(OSINFO_IS_PLATFORM(platform), NULL);
    g_return_val_if_fail(!filter || OSINFO_IS_FILTER(filter), NULL);

    OsinfoDeviceLinkList *newList = osinfo_devicelinklist_new();
    GList *tmp = platform->priv->deviceLinks;

    while (tmp) {
        OsinfoDeviceLink *devlink = OSINFO_DEVICELINK(tmp->data);

        if (!filter || osinfo_filter_matches(filter, OSINFO_ENTITY(devlink)))
            osinfo_list_add(OSINFO_LIST(newList), OSINFO_ENTITY(devlink));

        tmp = tmp->next;
    }

    return newList;
}


/**
 * osinfo_platform_add_device:
 * @platform: a platform entity
 * @dev: (transfer none): the device to associate
 *
 * Associate a device with a platform. The returned #OsinfoDeviceLink
 * can be used to record extra metadata against the link
 *
 * Returns: (transfer none): the device association
 */
OsinfoDeviceLink *osinfo_platform_add_device(OsinfoPlatform *platform, OsinfoDevice *dev)
{
    g_return_val_if_fail(OSINFO_IS_PLATFORM(platform), NULL);
    g_return_val_if_fail(OSINFO_IS_DEVICE(dev), NULL);

    OsinfoDeviceLink *devlink = osinfo_devicelink_new(dev);

    platform->priv->deviceLinks = g_list_prepend(platform->priv->deviceLinks,
                                                 devlink);

    return devlink;
}
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
