/*
 * libosinfo: A single hardware device
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Authors:
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <glib-object.h>
#include <osinfo/osinfo_entity.h>

#ifndef __OSINFO_DEVICE_H__
#define __OSINFO_DEVICE_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_DEVICE                  (osinfo_device_get_type ())
#define OSINFO_DEVICE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_DEVICE, OsinfoDevice))
#define OSINFO_IS_DEVICE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_DEVICE))
#define OSINFO_DEVICE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_DEVICE, OsinfoDeviceClass))
#define OSINFO_IS_DEVICE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_DEVICE))
#define OSINFO_DEVICE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_DEVICE, OsinfoDeviceClass))

typedef struct _OsinfoDevice        OsinfoDevice;

typedef struct _OsinfoDeviceClass   OsinfoDeviceClass;

typedef struct _OsinfoDevicePrivate OsinfoDevicePrivate;

#define OSINFO_DEVICE_PROP_VENDOR   "vendor"
#define OSINFO_DEVICE_PROP_PRODUCT  "product"
#define OSINFO_DEVICE_PROP_NAME     "name"
#define OSINFO_DEVICE_PROP_CLASS    "class"
#define OSINFO_DEVICE_PROP_BUS_TYPE "bus-type"

/* object */
struct _OsinfoDevice
{
    OsinfoEntity parent_instance;

    /* public */

    /* private */
    OsinfoDevicePrivate *priv;
};

/* class */
struct _OsinfoDeviceClass
{
    OsinfoEntityClass parent_class;

    /* class members */
};

GType osinfo_device_get_type(void);

OsinfoDevice *osinfo_device_new(const gchar *id);


const gchar *osinfo_device_get_vendor(OsinfoDevice *dev);
const gchar *osinfo_device_get_product(OsinfoDevice *dev);
const gchar *osinfo_device_get_bus_type(OsinfoDevice *dev);
const gchar *osinfo_device_get_class(OsinfoDevice *dev);
const gchar *osinfo_device_get_name(OsinfoDevice *dev);

#endif /* __OSINFO_DEVICE_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
