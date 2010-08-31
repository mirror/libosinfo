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

G_DEFINE_TYPE (OsinfoOsList, osinfo_oslist, OSINFO_TYPE_LIST);

#define OSINFO_OSLIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_OSLIST, OsinfoOsListPrivate))

static void osinfo_oslist_finalize (GObject *object);

struct _OsinfoOsListPrivate
{
    gboolean unused;
};

static void
osinfo_oslist_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_oslist_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_oslist_class_init (OsinfoOsListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_oslist_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoOsListPrivate));
}

static void
osinfo_oslist_init (OsinfoOsList *list)
{
    OsinfoOsListPrivate *priv;
    list->priv = priv = OSINFO_OSLIST_GET_PRIVATE(list);

}


OsinfoOsList *osinfo_oslist_new(void)
{
    return g_object_new(OSINFO_TYPE_OSLIST,
			"element-type", OSINFO_TYPE_OS,
			NULL);
}


OsinfoOsList *osinfo_oslist_new_copy(OsinfoOsList *source)
{
    OsinfoOsList *newList = osinfo_oslist_new();
    osinfo_list_add_all(OSINFO_LIST(newList),
			OSINFO_LIST(source));
    return newList;
}

OsinfoOsList *osinfo_oslist_new_filtered(OsinfoOsList *source, OsinfoFilter *filter)
{
    OsinfoOsList *newList = osinfo_oslist_new();
    osinfo_list_add_filtered(OSINFO_LIST(newList),
			     OSINFO_LIST(source),
			     filter);
    return newList;
}

OsinfoOsList *osinfo_oslist_new_intersection(OsinfoOsList *sourceOne, OsinfoOsList *sourceTwo)
{
    OsinfoOsList *newList = osinfo_oslist_new();
    osinfo_list_add_intersection(OSINFO_LIST(newList),
				 OSINFO_LIST(sourceOne),
				 OSINFO_LIST(sourceTwo));
    return newList;
}

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
