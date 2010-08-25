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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
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

#endif /* __OSINFO_DEVICE_H__ */
