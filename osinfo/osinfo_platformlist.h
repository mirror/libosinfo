/*
 * libosinfo: a list of platforms
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
#include <osinfo/osinfo_productlist.h>

#ifndef __OSINFO_PLATFORMLIST_H__
#define __OSINFO_PLATFORMLIST_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_PLATFORMLIST                  (osinfo_platformlist_get_type ())
#define OSINFO_PLATFORMLIST(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_PLATFORMLIST, OsinfoPlatformList))
#define OSINFO_IS_PLATFORMLIST(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_PLATFORMLIST))
#define OSINFO_PLATFORMLIST_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_PLATFORMLIST, OsinfoPlatformListClass))
#define OSINFO_IS_PLATFORMLIST_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_PLATFORMLIST))
#define OSINFO_PLATFORMLIST_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_PLATFORMLIST, OsinfoPlatformListClass))

typedef struct _OsinfoPlatformList        OsinfoPlatformList;

typedef struct _OsinfoPlatformListClass   OsinfoPlatformListClass;

typedef struct _OsinfoPlatformListPrivate OsinfoPlatformListPrivate;

/* object */
struct _OsinfoPlatformList
{
    OsinfoProductList parent_instance;

    /* public */

    /* private */
    OsinfoPlatformListPrivate *priv;
};

/* class */
struct _OsinfoPlatformListClass
{
    /*< private >*/
    OsinfoProductListClass parent_class;

    /* class members */
};

GType osinfo_platformlist_get_type(void);

OsinfoPlatformList *osinfo_platformlist_new(void);
G_DEPRECATED_FOR(osinfo_list_new_copy)
OsinfoPlatformList *osinfo_platformlist_new_copy(OsinfoPlatformList *source);
G_DEPRECATED_FOR(osinfo_list_new_filtered)
OsinfoPlatformList *osinfo_platformlist_new_filtered(OsinfoPlatformList *source, OsinfoFilter *filter);
G_DEPRECATED_FOR(osinfo_list_new_intersection)
OsinfoPlatformList *osinfo_platformlist_new_intersection(OsinfoPlatformList *sourceOne, OsinfoPlatformList *sourceTwo);
G_DEPRECATED_FOR(osinfo_list_new_union)
OsinfoPlatformList *osinfo_platformlist_new_union(OsinfoPlatformList *sourceOne, OsinfoPlatformList *sourceTwo);

#endif /* __OSINFO_PLATFORMLIST_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
