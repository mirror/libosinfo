/*
 * libosinfo: a list of devices
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
#include <osinfo/osinfo_filter.h>
#include <osinfo/osinfo_list.h>

#ifndef __OSINFO_DEVICELIST_H__
#define __OSINFO_DEVICELIST_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_DEVICELIST                  (osinfo_devicelist_get_type ())
#define OSINFO_DEVICELIST(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_DEVICELIST, OsinfoDeviceList))
#define OSINFO_IS_DEVICELIST(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_DEVICELIST))
#define OSINFO_DEVICELIST_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_DEVICELIST, OsinfoDeviceListClass))
#define OSINFO_IS_DEVICELIST_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_DEVICELIST))
#define OSINFO_DEVICELIST_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_DEVICELIST, OsinfoDeviceListClass))

typedef struct _OsinfoDeviceList        OsinfoDeviceList;

typedef struct _OsinfoDeviceListClass   OsinfoDeviceListClass;

typedef struct _OsinfoDeviceListPrivate OsinfoDeviceListPrivate;

/* object */
struct _OsinfoDeviceList
{
    OsinfoList parent_instance;

    /* public */

    /* private */
    OsinfoDeviceListPrivate *priv;
};

/* class */
struct _OsinfoDeviceListClass
{
    /*< private >*/
    OsinfoListClass parent_class;

    /* class members */
};

GType osinfo_devicelist_get_type(void);

OsinfoDeviceList *osinfo_devicelist_new(void);
G_DEPRECATED_FOR(osinfo_list_new_copy)
OsinfoDeviceList *osinfo_devicelist_new_copy(OsinfoDeviceList *source);
G_DEPRECATED_FOR(osinfo_list_new_filtered)
OsinfoDeviceList *osinfo_devicelist_new_filtered(OsinfoDeviceList *source, OsinfoFilter *filter);
G_DEPRECATED_FOR(osinfo_list_new_intersection)
OsinfoDeviceList *osinfo_devicelist_new_intersection(OsinfoDeviceList *sourceOne, OsinfoDeviceList *sourceTwo);
G_DEPRECATED_FOR(osinfo_list_new_union)
OsinfoDeviceList *osinfo_devicelist_new_union(OsinfoDeviceList *sourceOne, OsinfoDeviceList *sourceTwo);

#endif /* __OSINFO_DEVICELIST_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
