/*
 * libosinfo:
 *
 * Copyright (C) 2009-2012, 2014 Red Hat, Inc.
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
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <config.h>

#include <osinfo/osinfo.h>
#include <glib/gi18n-lib.h>

G_DEFINE_TYPE(OsinfoOsList, osinfo_oslist, OSINFO_TYPE_PRODUCTLIST);

#define OSINFO_OSLIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), OSINFO_TYPE_OSLIST, OsinfoOsListPrivate))

/**
 * SECTION:osinfo_oslist
 * @short_description: A list of os platforms
 * @see_also: #OsinfoList, #OsinfoOs
 *
 * #OsinfoOsList is a list specialization that stores
 * only #OsinfoOs objects.
 */

struct _OsinfoOsListPrivate
{
    gboolean unused;
};

static void
osinfo_oslist_finalize(GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS(osinfo_oslist_parent_class)->finalize(object);
}

/* Init functions */
static void
osinfo_oslist_class_init(OsinfoOsListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS(klass);

    g_klass->finalize = osinfo_oslist_finalize;
    g_type_class_add_private(klass, sizeof(OsinfoOsListPrivate));
}

static void
osinfo_oslist_init(OsinfoOsList *list)
{
    list->priv = OSINFO_OSLIST_GET_PRIVATE(list);
}


/**
 * osinfo_oslist_new:
 *
 * Construct a new os list that is initially empty.
 *
 * Returns: (transfer full): an empty os list
 */
OsinfoOsList *osinfo_oslist_new(void)
{
    return g_object_new(OSINFO_TYPE_OSLIST,
                        "element-type", OSINFO_TYPE_OS,
                        NULL);
}


/**
 * osinfo_oslist_new_copy:
 * @source: the os list to copy
 *
 * Construct a new os list that is filled with oss
 * from @source
 *
 * Returns: (transfer full): a copy of the os list
 * Deprecated: 0.2.2: Use osinfo_list_new_copy() instead.
 */
OsinfoOsList *osinfo_oslist_new_copy(OsinfoOsList *source)
{
    OsinfoOsList *newList = osinfo_oslist_new();
    osinfo_list_add_all(OSINFO_LIST(newList),
                        OSINFO_LIST(source));
    return newList;
}

/**
 * osinfo_oslist_new_filtered:
 * @source: the os list to copy
 * @filter: the filter to apply
 *
 * Construct a new os list that is filled with oss
 * from @source that match @filter
 *
 * Returns: (transfer full): a filtered copy of the os list
 * Deprecated: 0.2.2: Use osinfo_list_new_filtered() instead.
 */
OsinfoOsList *osinfo_oslist_new_filtered(OsinfoOsList *source, OsinfoFilter *filter)
{
    OsinfoOsList *newList = osinfo_oslist_new();
    osinfo_list_add_filtered(OSINFO_LIST(newList),
                             OSINFO_LIST(source),
                             filter);
    return newList;
}

/**
 * osinfo_oslist_new_intersection:
 * @sourceOne: the first os list to copy
 * @sourceTwo: the second os list to copy
 *
 * Construct a new os list that is filled with only the
 * oss that are present in both @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): an intersection of the two os lists
 * Deprecated: 0.2.2: Use osinfo_list_new_intersection() instead.
 */
OsinfoOsList *osinfo_oslist_new_intersection(OsinfoOsList *sourceOne, OsinfoOsList *sourceTwo)
{
    OsinfoOsList *newList = osinfo_oslist_new();
    osinfo_list_add_intersection(OSINFO_LIST(newList),
                                 OSINFO_LIST(sourceOne),
                                 OSINFO_LIST(sourceTwo));
    return newList;
}

/**
 * osinfo_oslist_new_union:
 * @sourceOne: the first os list to copy
 * @sourceTwo: the second os list to copy
 *
 * Construct a new os list that is filled with all the
 * oss that are present in either @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): a union of the two os lists
 * Deprecated: 0.2.2: Use osinfo_list_new_union() instead.
 */
OsinfoOsList *osinfo_oslist_new_union(OsinfoOsList *sourceOne, OsinfoOsList *sourceTwo)
{
    OsinfoOsList *newList = osinfo_oslist_new();
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
