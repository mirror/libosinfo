/*
 * libosinfo: a list of software products
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
#include <osinfo/osinfo_filter.h>
#include <osinfo/osinfo_list.h>

#ifndef __OSINFO_PRODUCTLIST_H__
#define __OSINFO_PRODUCTLIST_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_PRODUCTLIST                  (osinfo_productlist_get_type ())
#define OSINFO_PRODUCTLIST(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_PRODUCTLIST, OsinfoProductList))
#define OSINFO_IS_PRODUCTLIST(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_PRODUCTLIST))
#define OSINFO_PRODUCTLIST_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_PRODUCTLIST, OsinfoProductListClass))
#define OSINFO_IS_PRODUCTLIST_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_PRODUCTLIST))
#define OSINFO_PRODUCTLIST_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_PRODUCTLIST, OsinfoProductListClass))

typedef struct _OsinfoProductList        OsinfoProductList;

typedef struct _OsinfoProductListClass   OsinfoProductListClass;

typedef struct _OsinfoProductListPrivate OsinfoProductListPrivate;

/* object */
struct _OsinfoProductList
{
    OsinfoList parent_instance;

    /* public */

    /* private */
    OsinfoProductListPrivate *priv;
};

/* class */
struct _OsinfoProductListClass
{
    OsinfoListClass parent_class;

    /* class members */
};

GType osinfo_productlist_get_type(void);

OsinfoProductList *osinfo_productlist_new(void);
OsinfoProductList *osinfo_productlist_new_copy(OsinfoProductList *source);
OsinfoProductList *osinfo_productlist_new_filtered(OsinfoProductList *source, OsinfoFilter *filter);
OsinfoProductList *osinfo_productlist_new_intersection(OsinfoProductList *sourceOne, OsinfoProductList *sourceTwo);
OsinfoProductList *osinfo_productlist_new_union(OsinfoProductList *sourceOne, OsinfoProductList *sourceTwo);

#endif /* __OSINFO_PRODUCTLIST_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
