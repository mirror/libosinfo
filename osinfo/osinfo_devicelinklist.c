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

G_DEFINE_TYPE (OsinfoDeviceLinkList, osinfo_devicelinklist, OSINFO_TYPE_LIST);

#define OSINFO_DEVICELINKLIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_DEVICELINKLIST, OsinfoDeviceLinkListPrivate))

/**
 * SECTION:osinfo_devicelinklist
 * @short_description: A list of hardware devicelink
 * @see_also: #OsinfoList, #OsinfoDeviceLink
 *
 * #OsinfoDeviceLinkList is a list specialization that stores
 * only #OsinfoDeviceLink objects.
 */

struct _OsinfoDeviceLinkListPrivate
{
    gboolean unused;
};

static void
osinfo_devicelinklist_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_devicelinklist_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_devicelinklist_class_init (OsinfoDeviceLinkListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_devicelinklist_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoDeviceLinkListPrivate));
}

static void
osinfo_devicelinklist_init (OsinfoDeviceLinkList *list)
{
    OsinfoDeviceLinkListPrivate *priv;
    list->priv = priv = OSINFO_DEVICELINKLIST_GET_PRIVATE(list);

}


/**
 * osinfo_devicelinklist_new:
 *
 * Construct a new devicelink list that is initially empty.
 *
 * Returns: (transfer full): an empty devicelink list
 */
OsinfoDeviceLinkList *osinfo_devicelinklist_new(void)
{
    return g_object_new(OSINFO_TYPE_DEVICELINKLIST,
			"element-type", OSINFO_TYPE_DEVICELINK,
			NULL);
}


/**
 * osinfo_devicelinklist_new_copy:
 * @source: the devicelink list to copy
 *
 * Construct a new devicelink list that is filled with devicelinks
 * from @source
 *
 * Returns: (transfer full): a copy of the devicelink list
 */
OsinfoDeviceLinkList *osinfo_devicelinklist_new_copy(OsinfoDeviceLinkList *source)
{
    OsinfoDeviceLinkList *newList = osinfo_devicelinklist_new();
    osinfo_list_add_all(OSINFO_LIST(newList),
			OSINFO_LIST(source));
    return newList;
}


/**
 * osinfo_devicelinklist_new_filtered:
 * @source: the devicelink list to copy
 * @filter: the filter to apply
 *
 * Construct a new devicelink list that is filled with devicelinks
 * from @source that match @filter
 *
 * Returns: (transfer full): a filtered copy of the devicelink list
 */
OsinfoDeviceLinkList *osinfo_devicelinklist_new_filtered(OsinfoDeviceLinkList *source, OsinfoFilter *filter)
{
    OsinfoDeviceLinkList *newList = osinfo_devicelinklist_new();
    osinfo_list_add_filtered(OSINFO_LIST(newList),
			     OSINFO_LIST(source),
			     filter);
    return newList;
}

/**
 * osinfo_devicelinklist_new_intersection:
 * @sourceOne: the first devicelink list to copy
 * @sourceTwo: the second devicelink list to copy
 *
 * Construct a new devicelink list that is filled with only the
 * devicelinks that are present in both @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): an intersection of the two devicelink lists
 */
OsinfoDeviceLinkList *osinfo_devicelinklist_new_intersection(OsinfoDeviceLinkList *sourceOne, OsinfoDeviceLinkList *sourceTwo)
{
    OsinfoDeviceLinkList *newList = osinfo_devicelinklist_new();
    osinfo_list_add_intersection(OSINFO_LIST(newList),
				 OSINFO_LIST(sourceOne),
				 OSINFO_LIST(sourceTwo));
    return newList;
}

/**
 * osinfo_devicelinklist_new_union:
 * @sourceOne: the first devicelink list to copy
 * @sourceTwo: the second devicelink list to copy
 *
 * Construct a new devicelink list that is filled with all the
 * devicelinks that are present in either @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): a union of the two devicelink lists
 */
OsinfoDeviceLinkList *osinfo_devicelinklist_new_union(OsinfoDeviceLinkList *sourceOne, OsinfoDeviceLinkList *sourceTwo)
{
    OsinfoDeviceLinkList *newList = osinfo_devicelinklist_new();
    osinfo_list_add_union(OSINFO_LIST(newList),
			  OSINFO_LIST(sourceOne),
			  OSINFO_LIST(sourceTwo));
    return newList;
}


OsinfoDeviceList *osinfo_devicelinklist_get_devices(OsinfoDeviceLinkList *list,
                                                    OsinfoFilter *filter)
{
    OsinfoDeviceList *newList = osinfo_devicelist_new();
    int i;
    for (i = 0 ; i < osinfo_list_get_length(OSINFO_LIST(list)) ; i++) {
        OsinfoEntity *ent = osinfo_list_get_nth(OSINFO_LIST(list), i);
        if (!filter || osinfo_filter_matches(filter, ent))
            osinfo_list_add(OSINFO_LIST(newList), ent);
    }
    return newList;
}


/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
