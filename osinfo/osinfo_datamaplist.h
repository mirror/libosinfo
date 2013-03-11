/*
 * libosinfo: a list of installation install_datamap
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
 *   Zeeshan Ali <zeenix@redhat.com>
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <glib-object.h>
#include <osinfo/osinfo_list.h>

#ifndef __OSINFO_DATAMAPLIST_H__
#define __OSINFO_DATAMAPLIST_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_DATAMAPLIST                  (osinfo_datamaplist_get_type ())
#define OSINFO_DATAMAPLIST(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_DATAMAPLIST, OsinfoDatamapList))
#define OSINFO_IS_DATAMAPLIST(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_DATAMAPLIST))
#define OSINFO_DATAMAPLIST_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_DATAMAPLIST, OsinfoDatamapListClass))
#define OSINFO_IS_DATAMAPLIST_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_DATAMAPLIST))
#define OSINFO_DATAMAPLIST_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_DATAMAPLIST, OsinfoDatamapListClass))

typedef struct _OsinfoDatamapList        OsinfoDatamapList;

typedef struct _OsinfoDatamapListClass   OsinfoDatamapListClass;

typedef struct _OsinfoDatamapListPrivate OsinfoDatamapListPrivate;

/* object */
struct _OsinfoDatamapList
{
    OsinfoList parent_instance;

    /* public */

    /* private */
    OsinfoDatamapListPrivate *priv;
};

/* class */
struct _OsinfoDatamapListClass
{
    OsinfoListClass parent_class;

    /* class members */
};

GType osinfo_datamaplist_get_type(void);

OsinfoDatamapList *osinfo_datamaplist_new(void);

#endif /* __OSINFO_DATAMAPLIST_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
