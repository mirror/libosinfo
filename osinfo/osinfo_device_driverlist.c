/*
 * libosinfo: Device driver list
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
 *   Zeeshan Ali <zeenix@redhat.com>
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <config.h>

#include <osinfo/osinfo.h>
#include <glib/gi18n-lib.h>

G_DEFINE_TYPE (OsinfoDeviceDriverList, osinfo_device_driverlist, OSINFO_TYPE_LIST);

#define OSINFO_DEVICE_DRIVERLIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_DEVICE_DRIVERLIST, OsinfoDeviceDriverListPrivate))

/**
 * SECTION:osinfo_device_driverlist
 * @short_description: A list of device drivers
 * @see_also: #OsinfoList, #OsinfoDeviceDriver
 *
 * #OsinfoDeviceDriverList is a list specialization that stores only
 * #OsinfoDeviceDriver objects.
 */

struct _OsinfoDeviceDriverListPrivate
{
    gboolean unused;
};

static void
osinfo_device_driverlist_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_device_driverlist_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_device_driverlist_class_init (OsinfoDeviceDriverListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_device_driverlist_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoDeviceDriverListPrivate));
}

static void
osinfo_device_driverlist_init (OsinfoDeviceDriverList *list)
{
    OsinfoDeviceDriverListPrivate *priv;
    list->priv = priv = OSINFO_DEVICE_DRIVERLIST_GET_PRIVATE(list);

}

/**
 * osinfo_device_driverlist_new:
 *
 * Construct a new device driver list that is initially empty.
 *
 * Returns: (transfer full): an empty device driver list
 */
OsinfoDeviceDriverList *osinfo_device_driverlist_new(void)
{
    return g_object_new(OSINFO_TYPE_DEVICE_DRIVERLIST,
                        "element-type", OSINFO_TYPE_DEVICE_DRIVER,
                        NULL);
}

/**
 * osinfo_device_driverlist_new_copy:
 * @source: the device driver list to copy
 *
 * Construct a new device driver list that is filled with device drivers
 * from @source
 *
 * Returns: (transfer full): a copy of the device driver list
 */
OsinfoDeviceDriverList *osinfo_device_driverlist_new_copy(OsinfoDeviceDriverList *source)
{
    OsinfoDeviceDriverList *newList = osinfo_device_driverlist_new();
    osinfo_list_add_all(OSINFO_LIST(newList),
                        OSINFO_LIST(source));
    return newList;
}

/**
 * osinfo_device_driverlist_new_filtered:
 * @source: the device driver list to copy
 * @filter: the filter to apply
 *
 * Construct a new device driver list that is filled with device drivers
 * from @source that match @filter
 *
 * Returns: (transfer full): a filtered copy of the device driver list
 */
OsinfoDeviceDriverList *osinfo_device_driverlist_new_filtered(OsinfoDeviceDriverList *source,
                                                              OsinfoFilter *filter)
{
    OsinfoDeviceDriverList *newList = osinfo_device_driverlist_new();
    osinfo_list_add_filtered(OSINFO_LIST(newList),
                             OSINFO_LIST(source),
                             filter);
    return newList;
}

/**
 * osinfo_device_driverlist_new_intersection:
 * @sourceOne: the first device driver list to copy
 * @sourceTwo: the second device driver list to copy
 *
 * Construct a new device driver list that is filled with only the
 * device drivers that are present in both @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): an intersection of the two device driver lists
 */
OsinfoDeviceDriverList *osinfo_device_driverlist_new_intersection(OsinfoDeviceDriverList *sourceOne,
                                                                  OsinfoDeviceDriverList *sourceTwo)
{
    OsinfoDeviceDriverList *newList = osinfo_device_driverlist_new();
    osinfo_list_add_intersection(OSINFO_LIST(newList),
                                 OSINFO_LIST(sourceOne),
                                 OSINFO_LIST(sourceTwo));
    return newList;
}

/**
 * osinfo_device_driverlist_new_union:
 * @sourceOne: the first device driver list to copy
 * @sourceTwo: the second device driver list to copy
 *
 * Construct a new device driver list that is filled with all the
 * device drivers that are present in either @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): a union of the two device driver lists
 */
OsinfoDeviceDriverList *osinfo_device_driverlist_new_union(OsinfoDeviceDriverList *sourceOne,
                                                           OsinfoDeviceDriverList *sourceTwo)
{
    OsinfoDeviceDriverList *newList = osinfo_device_driverlist_new();
    osinfo_list_add_union(OSINFO_LIST(newList),
                          OSINFO_LIST(sourceOne),
                          OSINFO_LIST(sourceTwo));
    return newList;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
