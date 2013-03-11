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
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <config.h>

#include <osinfo/osinfo.h>
#include <string.h>
#include <libxml/tree.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libxslt/xsltInternals.h>

G_DEFINE_TYPE (OsinfoDatamap, osinfo_datamap, OSINFO_TYPE_ENTITY);

#define OSINFO_DATAMAP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_DATAMAP, OsinfoDatamapPrivate))

/**
 * SECTION:osinfo_datamap
 * @short_descripion: OS  datamap
 * @see_also: #OsinfoDatamap
 *
 * #OsinfoDatamap is an object for representing OS
 * datamaps. It is to translate generic osinfo values to OS
 * specific data.
 */

struct _OsinfoDatamapPrivate
{
    GHashTable *map;
    GHashTable *reverse_map;
};

static void
osinfo_datamap_finalize (GObject *object)
{
    OsinfoDatamap *map = OSINFO_DATAMAP(object);

    g_hash_table_unref(map->priv->map);
    g_hash_table_unref(map->priv->reverse_map);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_datamap_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_datamap_class_init (OsinfoDatamapClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_datamap_finalize;

    g_type_class_add_private (klass, sizeof (OsinfoDatamapPrivate));
}

static void
osinfo_datamap_init (OsinfoDatamap *list)
{
    list->priv = OSINFO_DATAMAP_GET_PRIVATE(list);
    list->priv->map = g_hash_table_new_full(g_str_hash,
                                            g_str_equal,
                                            g_free,
                                            g_free);
    list->priv->reverse_map = g_hash_table_new(g_str_hash, g_str_equal);
}


OsinfoDatamap *osinfo_datamap_new(const gchar *id)
{
    return g_object_new(OSINFO_TYPE_DATAMAP,
                        "id", id,
                        NULL);
}


void osinfo_datamap_insert(OsinfoDatamap *map,
                           const gchar *inval,
                           const gchar *outval)
{
    gchar *dup_inval;
    gchar *dup_outval;
    g_return_if_fail(OSINFO_IS_DATAMAP(map));
    g_return_if_fail(inval != NULL);

    dup_inval = g_strdup(inval);
    dup_outval = g_strdup(outval);
    g_hash_table_insert(map->priv->map, dup_inval, dup_outval);
    g_hash_table_insert(map->priv->reverse_map, dup_outval, dup_inval);
}

const gchar *osinfo_datamap_lookup(OsinfoDatamap *map,
                                   const gchar *inval)
{
    return g_hash_table_lookup(map->priv->map, inval);
}

const gchar *osinfo_datamap_reverse_lookup(OsinfoDatamap *map,
                                           const gchar *outval)
{
    return g_hash_table_lookup(map->priv->reverse_map, outval);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
