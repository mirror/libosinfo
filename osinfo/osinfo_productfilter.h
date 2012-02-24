/*
 * libosinfo: a mechanism to filter operating systems
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

#ifndef __OSINFO_PRODUCTFILTER_H__
#define __OSINFO_PRODUCTFILTER_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_PRODUCTFILTER                  (osinfo_productfilter_get_type ())
#define OSINFO_PRODUCTFILTER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_PRODUCTFILTER, OsinfoProductFilter))
#define OSINFO_IS_PRODUCTFILTER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_PRODUCTFILTER))
#define OSINFO_PRODUCTFILTER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_PRODUCTFILTER, OsinfoProductFilterClass))
#define OSINFO_IS_PRODUCTFILTER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_PRODUCTFILTER))
#define OSINFO_PRODUCTFILTER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_PRODUCTFILTER, OsinfoProductFilterClass))

typedef struct _OsinfoProductFilter        OsinfoProductFilter;

typedef struct _OsinfoProductFilterClass  OsinfoProductFilterClass;

typedef struct _OsinfoProductFilterPrivate OsinfoProductFilterPrivate;

/* object */
struct _OsinfoProductFilter
{
    OsinfoFilter parent_instance;

    /* public */

    /* private */
    OsinfoProductFilterPrivate *priv;
};

/* class */
struct _OsinfoProductFilterClass
{
    OsinfoFilterClass parent_class;

    /* class members */
};

GType osinfo_productfilter_get_type(void);

OsinfoProductFilter *osinfo_productfilter_new(void);

gint osinfo_productfilter_add_product_constraint(OsinfoProductFilter *productfilter, OsinfoProductRelationship relshp, OsinfoProduct *product);
void osinfo_productfilter_clear_product_constraint(OsinfoProductFilter *productfilter, OsinfoProductRelationship relshp);
void osinfo_productfilter_clear_product_constraints(OsinfoProductFilter *productfilter);

GList *osinfo_productfilter_get_product_constraint_values(OsinfoProductFilter *productfilter, OsinfoProductRelationship relshp);

void osinfo_productfilter_add_support_date_constraint(OsinfoProductFilter *productfilter, GDate *when);

#endif /* __OSINFO_PRODUCTFILTER_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
