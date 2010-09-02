/*
 * libosinfo:
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

#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoPlatformList, osinfo_platformlist, OSINFO_TYPE_LIST);

#define OSINFO_PLATFORMLIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_PLATFORMLIST, OsinfoPlatformListPrivate))

/**
 * SECTION:osinfo_platformlist
 * @short_description: A list of virtualization platforms
 * @see_also: #OsinfoList, #OsinfoPlatform
 *
 * #OsinfoPlatformList is a list specialization that stores
 * only #OsinfoPlatform objects.
 */

struct _OsinfoPlatformListPrivate
{
    gboolean unused;
};

static void
osinfo_platformlist_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_platformlist_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_platformlist_class_init (OsinfoPlatformListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_platformlist_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoPlatformListPrivate));
}

static void
osinfo_platformlist_init (OsinfoPlatformList *list)
{
    OsinfoPlatformListPrivate *priv;
    list->priv = priv = OSINFO_PLATFORMLIST_GET_PRIVATE(list);

}


/**
 * osinfo_platformlist_new:
 *
 * Construct a new platform list that is initially empty.
 *
 * Returns: (transfer full): an empty platform list
 */
OsinfoPlatformList *osinfo_platformlist_new(void)
{
    return g_object_new(OSINFO_TYPE_PLATFORMLIST,
			"element-type", OSINFO_TYPE_PLATFORM,
			NULL);
}

/**
 * osinfo_platformlist_new_copy:
 * @source: the platform list to copy
 *
 * Construct a new platform list that is filled with platforms
 * from @source
 *
 * Returns: (transfer full): a copy of the platform list
 */
OsinfoPlatformList *osinfo_platformlist_new_copy(OsinfoPlatformList *source)
{
    OsinfoPlatformList *newList = osinfo_platformlist_new();
    osinfo_list_add_all(OSINFO_LIST(newList),
			OSINFO_LIST(source));
    return newList;
}

/**
 * osinfo_platformlist_new_filtered:
 * @source: the platform list to copy
 * @filter: the filter to apply
 *
 * Construct a new platform list that is filled with platforms
 * from @source that match @filter
 *
 * Returns: (transfer full): a filtered copy of the platform list
 */
OsinfoPlatformList *osinfo_platformlist_new_filtered(OsinfoPlatformList *source, OsinfoFilter *filter)
{
    OsinfoPlatformList *newList = osinfo_platformlist_new();
    osinfo_list_add_filtered(OSINFO_LIST(newList),
			     OSINFO_LIST(source),
			     filter);
    return newList;
}

/**
 * osinfo_platformlist_new_intersection:
 * @sourceOne: the first platform list to copy
 * @sourceTwo: the second platform list to copy
 *
 * Construct a new platform list that is filled with only the
 * platforms that are present in both @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): an intersection of the two platform lists
 */
OsinfoPlatformList *osinfo_platformlist_new_intersection(OsinfoPlatformList *sourceOne, OsinfoPlatformList *sourceTwo)
{
    OsinfoPlatformList *newList = osinfo_platformlist_new();
    osinfo_list_add_intersection(OSINFO_LIST(newList),
				 OSINFO_LIST(sourceOne),
				 OSINFO_LIST(sourceTwo));
    return newList;
}

/**
 * osinfo_platformlist_new_union:
 * @sourceOne: the first platform list to copy
 * @sourceTwo: the second platform list to copy
 *
 * Construct a new platform list that is filled with all the
 * platforms that are present in either @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): a union of the two platform lists
 */
OsinfoPlatformList *osinfo_platformlist_new_union(OsinfoPlatformList *sourceOne, OsinfoPlatformList *sourceTwo)
{
    OsinfoPlatformList *newList = osinfo_platformlist_new();
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
