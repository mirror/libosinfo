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

#include <config.h>

#include <osinfo/osinfo.h>
#include <glib/gi18n-lib.h>

G_DEFINE_TYPE (OsinfoOsVariant, osinfo_os_variant, OSINFO_TYPE_ENTITY);

#define OSINFO_OS_VARIANT_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                      OSINFO_TYPE_OS_VARIANT, \
                                      OsinfoOsVariantPrivate))

/**
 * SECTION:osinfo_os_variant
 * @short_description: A variant of an OS
 * @see_also: #OsinfoOs
 *
 * #OsinfoOsVariant is an entity representing a variant of an operating system.
 */
struct _OsinfoOsVariantPrivate
{
    gboolean unused;
};

enum {
    PROP_0,

    PROP_NAME
};

static void
osinfo_os_variant_get_property (GObject    *object,
                                guint       property_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
    OsinfoOsVariant *variant = OSINFO_OS_VARIANT (object);

    switch (property_id) {
    case PROP_NAME:
        g_value_set_string (value,
                            osinfo_os_variant_get_name (variant));
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
osinfo_os_variant_set_property(GObject      *object,
                               guint         property_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
    OsinfoOsVariant *variant = OSINFO_OS_VARIANT (object);

    switch (property_id) {
    case PROP_NAME:
        osinfo_entity_set_param (OSINFO_ENTITY(variant),
                                 OSINFO_OS_VARIANT_PROP_NAME,
                                 g_value_get_string (value));
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

/* Init functions */
static void
osinfo_os_variant_class_init (OsinfoOsVariantClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);
    GParamSpec *pspec;

    g_klass->get_property = osinfo_os_variant_get_property;
    g_klass->set_property = osinfo_os_variant_set_property;
    g_type_class_add_private (klass, sizeof (OsinfoOsVariantPrivate));

    /**
     * OsinfoOsVariant:name:
     *
     * The name to this variant.
     */
    pspec = g_param_spec_string ("name",
                                 "Name",
                                 _("The name to this variant"),
                                 NULL /* default value */,
                                 G_PARAM_READWRITE |
                                 G_PARAM_STATIC_STRINGS);
    g_object_class_install_property (g_klass, PROP_NAME, pspec);
}

static void
osinfo_os_variant_init (OsinfoOsVariant *variant)
{
    variant->priv = OSINFO_OS_VARIANT_GET_PRIVATE(variant);
}

OsinfoOsVariant *osinfo_os_variant_new(const gchar *id)
{
    OsinfoOsVariant *variant;

    variant = g_object_new(OSINFO_TYPE_OS_VARIANT,
                           "id", id,
                           NULL);

    return variant;
}

/**
 * osinfo_os_variant_get_name:
 * @variant: an #OsinfoOsVariant instance
 *
 * The name of the @variant
 *
 * Returns: (transfer none): the name, or NULL
 */
const gchar *osinfo_os_variant_get_name(OsinfoOsVariant *variant)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(variant),
                                         OSINFO_OS_VARIANT_PROP_NAME);
}
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
