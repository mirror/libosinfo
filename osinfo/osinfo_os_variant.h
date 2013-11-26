/*
 * libosinfo: The variant of an OS
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
#include <gio/gio.h>
#include <osinfo/osinfo_entity.h>

#ifndef __OSINFO_OS_VARIANT_H__
#define __OSINFO_OS_VARIANT_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_OS_VARIANT                  (osinfo_os_variant_get_type ())
#define OSINFO_OS_VARIANT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_OS_VARIANT, OsinfoOsVariant))
#define OSINFO_IS_OS_VARIANT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_OS_VARIANT))
#define OSINFO_OS_VARIANT_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_OS_VARIANT, OsinfoOsVariantClass))
#define OSINFO_IS_OS_VARIANT_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_OS_VARIANT))
#define OSINFO_OS_VARIANT_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_OS_VARIANT, OsinfoOsVariantClass))

typedef struct _OsinfoOsVariant        OsinfoOsVariant;

typedef struct _OsinfoOsVariantClass   OsinfoOsVariantClass;

typedef struct _OsinfoOsVariantPrivate OsinfoOsVariantPrivate;

#define OSINFO_OS_VARIANT_PROP_NAME "name"

/* object */
struct _OsinfoOsVariant
{
    OsinfoEntity parent_instance;

    /* public */

    /* private */
    OsinfoOsVariantPrivate *priv;
};

/* class */
struct _OsinfoOsVariantClass
{
    /*< private >*/
    OsinfoEntityClass parent_class;

    /* class members */
};

GType osinfo_os_variant_get_type(void);

OsinfoOsVariant *osinfo_os_variant_new(const gchar *id);
const gchar *osinfo_os_variant_get_name(OsinfoOsVariant *variant);

#endif /* __OSINFO_OS_VARIANT_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
