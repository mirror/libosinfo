/*
 * osinfo: an software product
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPPRODUCTE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Bproductton, MA 02111-1307  USA
 *
 * Authors:
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <glib-object.h>
#include <osinfo/osinfo_productlist.h>

#ifndef __OSINFO_PRODUCT_H__
#define __OSINFO_PRODUCT_H__

/*
 * Type macrproduct.
 */
#define OSINFO_TYPE_PRODUCT                  (osinfo_product_get_type ())
#define OSINFO_PRODUCT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_PRODUCT, OsinfoProduct))
#define OSINFO_IS_PRODUCT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_PRODUCT))
#define OSINFO_PRODUCT_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_PRODUCT, OsinfoProductClass))
#define OSINFO_IS_PRODUCT_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_PRODUCT))
#define OSINFO_PRODUCT_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_PRODUCT, OsinfoProductClass))

typedef struct _OsinfoProduct        OsinfoProduct;

typedef struct _OsinfoProductClass   OsinfoProductClass;

typedef struct _OsinfoProductPrivate OsinfoProductPrivate;

#define OSINFO_PRODUCT_PROP_VENDOR   "vendor"
#define OSINFO_PRODUCT_PROP_VERSION  "version"
#define OSINFO_PRODUCT_PROP_SHORT_ID "short-id"
#define OSINFO_PRODUCT_PROP_NAME     "name"

/* object */
struct _OsinfoProduct
{
    OsinfoEntity parent_instance;

    /* public */

    /* private */
    OsinfoProductPrivate *priv;
};

/* class */
struct _OsinfoProductClass
{
    OsinfoEntityClass parent_class;

    /* class members */
};

typedef enum {
    OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM,
    OSINFO_PRODUCT_RELATIONSHIP_UPGRADES,
    OSINFO_PRODUCT_RELATIONSHIP_CLONES,
} OsinfoProductRelationship;


GType osinfo_product_get_type(void);

OsinfoProductList *osinfo_product_get_related(OsinfoProduct *product, OsinfoProductRelationship relshp);

void osinfo_product_add_related(OsinfoProduct *product, OsinfoProductRelationship relshp, OsinfoProduct *otherproduct);

const gchar *osinfo_product_get_vendor(OsinfoProduct *prod);
const gchar *osinfo_product_get_version(OsinfoProduct *prod);
const gchar *osinfo_product_get_short_id(OsinfoProduct *prod);
const gchar *osinfo_product_get_name(OsinfoProduct *prod);

#endif /* __OSINFO_PRODUCT_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
