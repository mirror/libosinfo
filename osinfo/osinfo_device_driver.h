/*
 * libosinfo: Device driver
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
 */

#include <glib-object.h>
#include <gio/gio.h>
#include <osinfo/osinfo_entity.h>
#include <osinfo/osinfo_devicelist.h>

#ifndef __OSINFO_DEVICE_DRIVER_H__
#define __OSINFO_DEVICE_DRIVER_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_DEVICE_DRIVER                  (osinfo_device_driver_get_type ())
#define OSINFO_DEVICE_DRIVER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                OSINFO_TYPE_DEVICE_DRIVER, OsinfoDeviceDriver))
#define OSINFO_IS_DEVICE_DRIVER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                                OSINFO_TYPE_DEVICE_DRIVER))
#define OSINFO_DEVICE_DRIVER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                                OSINFO_TYPE_DEVICE_DRIVER, OsinfoDeviceDriverClass))
#define OSINFO_IS_DEVICE_DRIVER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                                                OSINFO_TYPE_DEVICE_DRIVER))
#define OSINFO_DEVICE_DRIVER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                                OSINFO_TYPE_DEVICE_DRIVER, OsinfoDeviceDriverClass))

typedef struct _OsinfoDeviceDriver        OsinfoDeviceDriver;

typedef struct _OsinfoDeviceDriverClass   OsinfoDeviceDriverClass;

typedef struct _OsinfoDeviceDriverPrivate OsinfoDeviceDriverPrivate;

#define OSINFO_DEVICE_DRIVER_PROP_ARCHITECTURE    "arch"
#define OSINFO_DEVICE_DRIVER_PROP_LOCATION        "location"
#define OSINFO_DEVICE_DRIVER_PROP_PRE_INSTALLABLE "pre-installable"
#define OSINFO_DEVICE_DRIVER_PROP_FILE            "file"
#define OSINFO_DEVICE_DRIVER_PROP_DEVICE          "device"

/* object */
struct _OsinfoDeviceDriver
{
    OsinfoEntity parent_instance;

    /* public */

    /* private */
    OsinfoDeviceDriverPrivate *priv;
};

/* class */
struct _OsinfoDeviceDriverClass
{
    /*< private >*/
    OsinfoEntityClass parent_class;

    /* class members */
};

GType osinfo_device_driver_get_type(void);

const gchar *osinfo_device_driver_get_architecture(OsinfoDeviceDriver *driver);
const gchar *osinfo_device_driver_get_location(OsinfoDeviceDriver *driver);
gboolean osinfo_device_driver_get_pre_installable(OsinfoDeviceDriver *driver);
GList *osinfo_device_driver_get_files(OsinfoDeviceDriver *driver);
OsinfoDeviceList *osinfo_device_driver_get_devices(OsinfoDeviceDriver *driver);

#endif /* __OSINFO_DEVICE_DRIVER_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
