/*
 * libosinfo: OS data map
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

#include <glib-object.h>

#ifndef __OSINFO_DATAMAP_H__
#define __OSINFO_DATAMAP_H__

#include <osinfo/osinfo_entity.h>

/*
 * Type macros.
 */
#define OSINFO_TYPE_DATAMAP                  (osinfo_datamap_get_type ())
#define OSINFO_DATAMAP(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_DATAMAP, OsinfoDatamap))
#define OSINFO_IS_DATAMAP(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_DATAMAP))
#define OSINFO_DATAMAP_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_DATAMAP, OsinfoDatamapClass))
#define OSINFO_IS_DATAMAP_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_DATAMAP))
#define OSINFO_DATAMAP_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_DATAMAP, OsinfoDatamapClass))

typedef struct _OsinfoDatamap        OsinfoDatamap;
typedef struct _OsinfoDatamapClass   OsinfoDatamapClass;
typedef struct _OsinfoDatamapPrivate OsinfoDatamapPrivate;

/* object */
struct _OsinfoDatamap
{
    OsinfoEntity parent_instance;

    /* public */

    /* private */
    OsinfoDatamapPrivate *priv;
};

/* class */
struct _OsinfoDatamapClass
{
    OsinfoEntityClass parent_class;

    /* class members */
};

GType osinfo_datamap_get_type(void);

OsinfoDatamap *osinfo_datamap_new(const gchar *id);

void osinfo_datamap_insert(OsinfoDatamap *map,
                           const gchar *inval,
                           const gchar *outval);

const gchar *osinfo_datamap_lookup(OsinfoDatamap *map,
                                   const gchar *inval);
const gchar *osinfo_datamap_reverse_lookup(OsinfoDatamap *map,
                                           const gchar *outval);

#endif /* __OSINFO_DATAMAP_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
