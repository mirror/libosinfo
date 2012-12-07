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

#include <glib-object.h>
#include <osinfo/osinfo_list.h>

#ifndef __OSINFO_DEVICE_DRIVERLIST_H__
#define __OSINFO_DEVICE_DRIVERLIST_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_DEVICE_DRIVERLIST                  (osinfo_device_driverlist_get_type ())
#define OSINFO_DEVICE_DRIVERLIST(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                        OSINFO_TYPE_DEVICE_DRIVERLIST,     \
                                                        OsinfoDeviceDriverList))
#define OSINFO_IS_DEVICE_DRIVERLIST(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                                        OSINFO_TYPE_DEVICE_DRIVERLIST))
#define OSINFO_DEVICE_DRIVERLIST_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                                        OSINFO_TYPE_DEVICE_DRIVERLIST, \
                                                        OsinfoDeviceDriverListClass))
#define OSINFO_IS_DEVICE_DRIVERLIST_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                                                        OSINFO_TYPE_DEVICE_DRIVERLIST))
#define OSINFO_DEVICE_DRIVERLIST_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                                        OSINFO_TYPE_DEVICE_DRIVERLIST, \
                                                        OsinfoDeviceDriverListClass))

typedef struct _OsinfoDeviceDriverList        OsinfoDeviceDriverList;

typedef struct _OsinfoDeviceDriverListClass   OsinfoDeviceDriverListClass;

typedef struct _OsinfoDeviceDriverListPrivate OsinfoDeviceDriverListPrivate;

/* object */
struct _OsinfoDeviceDriverList
{
    OsinfoList parent_instance;

    /* public */

    /* private */
    OsinfoDeviceDriverListPrivate *priv;
};

/* class */
struct _OsinfoDeviceDriverListClass
{
    /*< private >*/
    OsinfoListClass parent_class;

    /* class members */
};

GType osinfo_device_driverlist_get_type(void);

OsinfoDeviceDriverList *osinfo_device_driverlist_new(void);
OsinfoDeviceDriverList *osinfo_device_driverlist_new_copy(OsinfoDeviceDriverList *source);
OsinfoDeviceDriverList *osinfo_device_driverlist_new_filtered(OsinfoDeviceDriverList *source,
                                                              OsinfoFilter *filter);
OsinfoDeviceDriverList *osinfo_device_driverlist_new_intersection(OsinfoDeviceDriverList *sourceOne,
                                                                  OsinfoDeviceDriverList *sourceTwo);
OsinfoDeviceDriverList *osinfo_device_driverlist_new_union(OsinfoDeviceDriverList *sourceOne,
                                                           OsinfoDeviceDriverList *sourceTwo);

#endif /* __OSINFO_DEVICE_DRIVERLIST_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
