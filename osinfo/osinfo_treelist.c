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

G_DEFINE_TYPE (OsinfoTreeList, osinfo_treelist, OSINFO_TYPE_LIST);

#define OSINFO_TREELIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_TREELIST, OsinfoTreeListPrivate))

/**
 * SECTION:osinfo_treelist
 * @short_description: A list of installation tree
 * @see_also: #OsinfoList, #OsinfoTree
 *
 * #OsinfoTreeList is a list specialization that stores
 * only #OsinfoTree objects.
 */

struct _OsinfoTreeListPrivate
{
    gboolean unused;
};

static void
osinfo_treelist_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_treelist_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_treelist_class_init (OsinfoTreeListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_treelist_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoTreeListPrivate));
}

static void
osinfo_treelist_init (OsinfoTreeList *list)
{
    list->priv = OSINFO_TREELIST_GET_PRIVATE(list);
}

/**
 * osinfo_treelist_new:
 *
 * Construct a new tree list that is initially empty.
 *
 * Returns: (transfer full): an empty tree list
 */
OsinfoTreeList *osinfo_treelist_new(void)
{
    return g_object_new(OSINFO_TYPE_TREELIST,
                        "element-type", OSINFO_TYPE_TREE,
                        NULL);
}

/**
 * osinfo_treelist_new_copy:
 * @source: the tree list to copy
 *
 * Construct a new tree list that is filled with trees
 * from @source
 *
 * Returns: (transfer full): a copy of the tree list
 * Deprecated: 0.2.2: Use osinfo_list_new_copy() instead.
 */
OsinfoTreeList *osinfo_treelist_new_copy(OsinfoTreeList *source)
{
    OsinfoTreeList *newList = osinfo_treelist_new();
    osinfo_list_add_all(OSINFO_LIST(newList),
                        OSINFO_LIST(source));
    return newList;
}

/**
 * osinfo_treelist_new_filtered:
 * @source: the tree list to copy
 * @filter: the filter to apply
 *
 * Construct a new tree list that is filled with trees
 * from @source that match @filter
 *
 * Returns: (transfer full): a filtered copy of the tree list
 * Deprecated: 0.2.2: Use osinfo_list_new_filtered() instead.
 */
OsinfoTreeList *osinfo_treelist_new_filtered(OsinfoTreeList *source,
                                               OsinfoFilter *filter)
{
    OsinfoTreeList *newList = osinfo_treelist_new();
    osinfo_list_add_filtered(OSINFO_LIST(newList),
                             OSINFO_LIST(source),
                             filter);
    return newList;
}

/**
 * osinfo_treelist_new_intersection:
 * @sourceOne: the first tree list to copy
 * @sourceTwo: the second tree list to copy
 *
 * Construct a new tree list that is filled with only the
 * trees that are present in both @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): an intersection of the two tree lists
 * Deprecated: 0.2.2: Use osinfo_list_new_intersection() instead.
 */
OsinfoTreeList *osinfo_treelist_new_intersection(OsinfoTreeList *sourceOne,
                                                   OsinfoTreeList *sourceTwo)
{
    OsinfoTreeList *newList = osinfo_treelist_new();
    osinfo_list_add_intersection(OSINFO_LIST(newList),
                                 OSINFO_LIST(sourceOne),
                                 OSINFO_LIST(sourceTwo));
    return newList;
}

/**
 * osinfo_treelist_new_union:
 * @sourceOne: the first tree list to copy
 * @sourceTwo: the second tree list to copy
 *
 * Construct a new tree list that is filled with all the
 * trees that are present in either @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): a union of the two tree lists
 * Deprecated: 0.2.2: Use osinfo_list_new_union() instead.
 */
OsinfoTreeList *osinfo_treelist_new_union(OsinfoTreeList *sourceOne,
                                          OsinfoTreeList *sourceTwo)
{
    OsinfoTreeList *newList = osinfo_treelist_new();
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
