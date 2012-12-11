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

G_DEFINE_TYPE (OsinfoDatamapList, osinfo_datamaplist, OSINFO_TYPE_LIST);

#define OSINFO_DATAMAPLIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_DATAMAPLIST, OsinfoDatamapListPrivate))

/**
 * SECTION:osinfo_datamaplist
 * @short_description: A list of datamaps
 * @see_also: #OsinfoList, #OsinfoDatamap
 *
 * #OsinfoDatamapList is a list specialization that stores
 * only #OsinfoDatamap objects.
 */

struct _OsinfoDatamapListPrivate
{
    gboolean unused;
};

static void
osinfo_datamaplist_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_datamaplist_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_datamaplist_class_init (OsinfoDatamapListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_datamaplist_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoDatamapListPrivate));
}

static void
osinfo_datamaplist_init (OsinfoDatamapList *list)
{
    OsinfoDatamapListPrivate *priv;
    list->priv = priv = OSINFO_DATAMAPLIST_GET_PRIVATE(list);

}

/**
 * osinfo_datamaplist_new:
 *
 * Construct a new install_datamap list that is initially empty.
 *
 * Returns: (transfer full): an empty install_datamap list
 */
OsinfoDatamapList *osinfo_datamaplist_new(void)
{
    return g_object_new(OSINFO_TYPE_DATAMAPLIST,
                        "element-type", OSINFO_TYPE_DATAMAP,
                        NULL);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
