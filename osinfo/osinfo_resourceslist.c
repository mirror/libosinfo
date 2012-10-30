/*
 * libosinfo:
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

#include <config.h>

#include <osinfo/osinfo.h>
#include <glib/gi18n-lib.h>

G_DEFINE_TYPE (OsinfoResourcesList, osinfo_resourceslist, OSINFO_TYPE_LIST);

#define OSINFO_RESOURCESLIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                               OSINFO_TYPE_RESOURCESLIST,          \
                                               OsinfoResourcesListPrivate))

/**
 * SECTION:osinfo_resourceslist
 * @short_description: A list of installation resources
 * @see_also: #OsinfoList, #OsinfoResources
 *
 * #OsinfoResourcesList is a list specialization that stores
 * only #OsinfoResources objects.
 */

struct _OsinfoResourcesListPrivate
{
    gboolean unused;
};

static void
osinfo_resourceslist_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_resourceslist_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_resourceslist_class_init (OsinfoResourcesListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_resourceslist_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoResourcesListPrivate));
}

static void
osinfo_resourceslist_init (OsinfoResourcesList *list)
{
    OsinfoResourcesListPrivate *priv;
    list->priv = priv = OSINFO_RESOURCESLIST_GET_PRIVATE(list);

}

/**
 * osinfo_resourceslist_new:
 *
 * Construct a new resources list that is initially empty.
 *
 * Returns: (transfer full): an empty resources list
 */
OsinfoResourcesList *osinfo_resourceslist_new(void)
{
    return g_object_new(OSINFO_TYPE_RESOURCESLIST,
                        "element-type", OSINFO_TYPE_RESOURCES,
                        NULL);
}

/**
 * osinfo_resourceslist_new_copy:
 * @source: the resources list to copy
 *
 * Construct a new resources list that is filled with resources instances
 * from @source
 *
 * Returns: (transfer full): a copy of the resources list
 */
OsinfoResourcesList *osinfo_resourceslist_new_copy(OsinfoResourcesList *source)
{
    OsinfoResourcesList *newList = osinfo_resourceslist_new();
    osinfo_list_add_all(OSINFO_LIST(newList),
                        OSINFO_LIST(source));
    return newList;
}

/**
 * osinfo_resourceslist_new_filtered:
 * @source: the resources list to copy
 * @filter: the filter to apply
 *
 * Construct a new resources list that is filled with resources instances
 * from @source that match @filter
 *
 * Returns: (transfer full): a filtered copy of the resources list
 */
OsinfoResourcesList *osinfo_resourceslist_new_filtered
                                (OsinfoResourcesList *source,
                                 OsinfoFilter *filter)
{
    OsinfoResourcesList *newList = osinfo_resourceslist_new();
    osinfo_list_add_filtered(OSINFO_LIST(newList),
                             OSINFO_LIST(source),
                             filter);
    return newList;
}

/**
 * osinfo_resourceslist_new_intersection:
 * @sourceOne: the first resources list to copy
 * @sourceTwo: the second resources list to copy
 *
 * Construct a new resources list that is filled with only the
 * resources instances that are present in both @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): an intersection of the two resources lists
 */
OsinfoResourcesList *osinfo_resourceslist_new_intersection
                                (OsinfoResourcesList *sourceOne,
                                 OsinfoResourcesList *sourceTwo)
{
    OsinfoResourcesList *newList = osinfo_resourceslist_new();
    osinfo_list_add_intersection(OSINFO_LIST(newList),
                                 OSINFO_LIST(sourceOne),
                                 OSINFO_LIST(sourceTwo));
    return newList;
}

/**
 * osinfo_resourceslist_new_union:
 * @sourceOne: the first resources list to copy
 * @sourceTwo: the second resources list to copy
 *
 * Construct a new resources list that is filled with all the
 * resources instances that are present in either @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): a union of the two resources lists
 */
OsinfoResourcesList *osinfo_resourceslist_new_union
                                (OsinfoResourcesList *sourceOne,
                                 OsinfoResourcesList *sourceTwo)
{
    OsinfoResourcesList *newList = osinfo_resourceslist_new();
    osinfo_list_add_union(OSINFO_LIST(newList),
                          OSINFO_LIST(sourceOne),
                          OSINFO_LIST(sourceTwo));
    return newList;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
