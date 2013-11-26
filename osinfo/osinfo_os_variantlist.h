/*
 * libosinfo: a list of OS variants
 *
 * Copyright (C) 2013 Red Hat, Inc.
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
 */

#include <glib-object.h>
#include <osinfo/osinfo_list.h>

#ifndef __OSINFO_OS_VARIANTLIST_H__
#define __OSINFO_OS_VARIANTLIST_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_OS_VARIANTLIST                  (osinfo_os_variantlist_get_type ())
#define OSINFO_OS_VARIANTLIST(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_OS_VARIANTLIST, OsinfoOsVariantList))
#define OSINFO_IS_OS_VARIANTLIST(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_OS_VARIANTLIST))
#define OSINFO_OS_VARIANTLIST_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_OS_VARIANTLIST, OsinfoOsVariantListClass))
#define OSINFO_IS_OS_VARIANTLIST_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_OS_VARIANTLIST))
#define OSINFO_OS_VARIANTLIST_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_OS_VARIANTLIST, OsinfoOsVariantListClass))

typedef struct _OsinfoOsVariantList        OsinfoOsVariantList;

typedef struct _OsinfoOsVariantListClass   OsinfoOsVariantListClass;

typedef struct _OsinfoOsVariantListPrivate OsinfoOsVariantListPrivate;

/* object */
struct _OsinfoOsVariantList
{
    OsinfoList parent_instance;

    /* public */

    /* private */
    OsinfoOsVariantListPrivate *priv;
};

/* class */
struct _OsinfoOsVariantListClass
{
    OsinfoListClass parent_class;

    /* class members */
};

GType osinfo_os_variantlist_get_type(void);

OsinfoOsVariantList *osinfo_os_variantlist_new(void);

#endif /* __OSINFO_OS_VARIANTLIST_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
