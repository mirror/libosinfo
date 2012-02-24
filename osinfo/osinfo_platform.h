/*
 * libosinfo: a virtualization platform
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
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <glib-object.h>
#include <osinfo/osinfo_product.h>
#include <osinfo/osinfo_device.h>
#include <osinfo/osinfo_devicelist.h>

#ifndef __OSINFO_PLATFORM_H__
#define __OSINFO_PLATFORM_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_PLATFORM                  (osinfo_platform_get_type ())
#define OSINFO_PLATFORM(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_PLATFORM, OsinfoPlatform))
#define OSINFO_IS_PLATFORM(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_PLATFORM))
#define OSINFO_PLATFORM_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_PLATFORM, OsinfoPlatformClass))
#define OSINFO_IS_PLATFORM_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_PLATFORM))
#define OSINFO_PLATFORM_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_PLATFORM, OsinfoPlatformClass))

typedef struct _OsinfoPlatform        OsinfoPlatform;

typedef struct _OsinfoPlatformClass   OsinfoPlatformClass;

typedef struct _OsinfoPlatformPrivate OsinfoPlatformPrivate;

/* object */
struct _OsinfoPlatform
{
    OsinfoProduct parent_instance;

    /* public */

    /* private */
    OsinfoPlatformPrivate *priv;
};

/* class */
struct _OsinfoPlatformClass
{
    OsinfoProductClass parent_class;

    /* class members */
};

GType osinfo_platform_get_type(void);

OsinfoPlatform *osinfo_platform_new(const gchar *id);

OsinfoDeviceList *osinfo_platform_get_devices(OsinfoPlatform *platform, OsinfoFilter *filter);
OsinfoDeviceLinkList *osinfo_platform_get_device_links(OsinfoPlatform *platform, OsinfoFilter *filter);

OsinfoDeviceLink *osinfo_platform_add_device(OsinfoPlatform *platform, OsinfoDevice *dev);

#endif /* __OSINFO_PLATFORM_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
