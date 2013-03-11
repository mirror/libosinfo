/*
 * libosinfo: a list of minimum/recommended resources
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
 *   Zeeshan Ali <zeenix@redhat.com>
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <glib-object.h>
#include <osinfo/osinfo_list.h>

#ifndef __OSINFO_RESOURCESLIST_H__
#define __OSINFO_RESOURCESLIST_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_RESOURCESLIST                  (osinfo_resourceslist_get_type ())
#define OSINFO_RESOURCESLIST(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                    OSINFO_TYPE_RESOURCESLIST,         \
                                                    OsinfoResourcesList))
#define OSINFO_IS_RESOURCESLIST(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                                    OSINFO_TYPE_RESOURCESLIST))
#define OSINFO_RESOURCESLIST_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                                    OSINFO_TYPE_RESOURCESLIST,         \
                                                    OsinfoResourcesListClass))
#define OSINFO_IS_RESOURCESLIST_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                                    OSINFO_TYPE_RESOURCESLIST))
#define OSINFO_RESOURCESLIST_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                                    OSINFO_TYPE_RESOURCESLIST,         \
                                                    OsinfoResourcesListClass))

typedef struct _OsinfoResourcesList        OsinfoResourcesList;

typedef struct _OsinfoResourcesListClass   OsinfoResourcesListClass;

typedef struct _OsinfoResourcesListPrivate OsinfoResourcesListPrivate;

/* object */
struct _OsinfoResourcesList
{
    OsinfoList parent_instance;

    /* public */

    /* private */
    OsinfoResourcesListPrivate *priv;
};

/* class */
struct _OsinfoResourcesListClass
{
    /*< private >*/
    OsinfoListClass parent_class;

    /* class members */
};

GType osinfo_resourceslist_get_type(void);

OsinfoResourcesList *osinfo_resourceslist_new(void);
G_DEPRECATED_FOR(osinfo_list_new_copy)
OsinfoResourcesList *osinfo_resourceslist_new_copy(OsinfoResourcesList *source);
G_DEPRECATED_FOR(osinfo_list_new_filtered)
OsinfoResourcesList *osinfo_resourceslist_new_filtered
                                (OsinfoResourcesList *source,
                                 OsinfoFilter *filter);
G_DEPRECATED_FOR(osinfo_list_new_intersection)
OsinfoResourcesList *osinfo_resourceslist_new_intersection
                                (OsinfoResourcesList *sourceOne,
                                 OsinfoResourcesList *sourceTwo);
G_DEPRECATED_FOR(osinfo_list_new_intersection)
OsinfoResourcesList *osinfo_resourceslist_new_union
                                (OsinfoResourcesList *sourceOne,
                                 OsinfoResourcesList *sourceTwo);

#endif /* __OSINFO_RESOURCESLIST_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
