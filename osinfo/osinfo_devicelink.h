/*
 * libosinfo: A reference to a hardware device
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
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <glib-object.h>
#include <osinfo/osinfo_entity.h>

#ifndef __OSINFO_DEVICELINK_H__
#define __OSINFO_DEVICELINK_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_DEVICELINK                  (osinfo_devicelink_get_type ())
#define OSINFO_DEVICELINK(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_DEVICELINK, OsinfoDeviceLink))
#define OSINFO_IS_DEVICELINK(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_DEVICELINK))
#define OSINFO_DEVICELINK_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_DEVICELINK, OsinfoDeviceLinkClass))
#define OSINFO_IS_DEVICELINK_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_DEVICELINK))
#define OSINFO_DEVICELINK_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_DEVICELINK, OsinfoDeviceLinkClass))

typedef struct _OsinfoDeviceLink        OsinfoDeviceLink;

typedef struct _OsinfoDeviceLinkClass   OsinfoDeviceLinkClass;

typedef struct _OsinfoDeviceLinkPrivate OsinfoDeviceLinkPrivate;

#define OSINFO_DEVICELINK_PROP_DRIVER   "driver"

/* object */
struct _OsinfoDeviceLink
{
    OsinfoEntity parent_instance;

    /* public */

    /* private */
    OsinfoDeviceLinkPrivate *priv;
};

/* class */
struct _OsinfoDeviceLinkClass
{
    /*< private >*/
    OsinfoEntityClass parent_class;

    /* class members */
};

GType osinfo_devicelink_get_type(void);

OsinfoDeviceLink *osinfo_devicelink_new(OsinfoDevice *target);

OsinfoDevice *osinfo_devicelink_get_target(OsinfoDeviceLink *link);

const gchar *osinfo_devicelink_get_driver(OsinfoDeviceLink *link);

#endif /* __OSINFO_DEVICELINK_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
