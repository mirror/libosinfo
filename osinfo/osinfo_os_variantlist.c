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

#include <config.h>

#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoOsVariantList, osinfo_os_variantlist, OSINFO_TYPE_LIST);

#define OSINFO_OS_VARIANTLIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_OS_VARIANTLIST, OsinfoOsVariantListPrivate))

/**
 * SECTION:osinfo_os_variantlist
 * @short_description: A list of OS variants
 * @see_also: #OsinfoList, #OsinfoOsVariant
 *
 * #OsinfoOsVariantList is a list specialization that stores
 * only #OsinfoOsVariant objects.
 */

struct _OsinfoOsVariantListPrivate
{
    gboolean unused;
};

/* Init functions */
static void
osinfo_os_variantlist_class_init (OsinfoOsVariantListClass *klass)
{
    g_type_class_add_private (klass, sizeof (OsinfoOsVariantListPrivate));
}

static void
osinfo_os_variantlist_init (OsinfoOsVariantList *list)
{
    list->priv = OSINFO_OS_VARIANTLIST_GET_PRIVATE(list);
}

/**
 * osinfo_os_variantlist_new:
 *
 * Construct a new install_variant list that is initially empty.
 *
 * Returns: (transfer full): an empty install_variant list
 */
OsinfoOsVariantList *osinfo_os_variantlist_new(void)
{
    return g_object_new(OSINFO_TYPE_OS_VARIANTLIST,
                        "element-type", OSINFO_TYPE_OS_VARIANT,
                        NULL);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
