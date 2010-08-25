/*
 * libosinfo: a list of hypervisors
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

#include <glib-object.h>
#include <osinfo/osinfo_filter.h>
#include <osinfo/osinfo_list.h>

#ifndef __OSINFO_HYPERVISORLIST_H__
#define __OSINFO_HYPERVISORLIST_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_HYPERVISORLIST                  (osinfo_hypervisorlist_get_type ())
#define OSINFO_HYPERVISORLIST(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_HYPERVISORLIST, OsinfoHypervisorList))
#define OSINFO_IS_HYPERVISORLIST(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_HYPERVISORLIST))
#define OSINFO_HYPERVISORLIST_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_HYPERVISORLIST, OsinfoHypervisorListClass))
#define OSINFO_IS_HYPERVISORLIST_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_HYPERVISORLIST))
#define OSINFO_HYPERVISORLIST_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_HYPERVISORLIST, OsinfoHypervisorListClass))

typedef struct _OsinfoHypervisorList        OsinfoHypervisorList;

typedef struct _OsinfoHypervisorListClass   OsinfoHypervisorListClass;

typedef struct _OsinfoHypervisorListPrivate OsinfoHypervisorListPrivate;

/* object */
struct _OsinfoHypervisorList
{
    OsinfoList parent_instance;

    /* public */

    /* private */
    OsinfoHypervisorListPrivate *priv;
};

/* class */
struct _OsinfoHypervisorListClass
{
    OsinfoListClass parent_class;

    /* class members */
};

GType osinfo_hypervisorlist_get_type(void);

OsinfoHypervisorList *osinfo_hypervisorlist_new(void);
OsinfoHypervisorList *osinfo_hypervisorlist_new_filtered(OsinfoHypervisorList *source, OsinfoFilter *filter);
OsinfoHypervisorList *osinfo_hypervisorlist_new_intersection(OsinfoHypervisorList *sourceOne, OsinfoHypervisorList *sourceTwo);
OsinfoHypervisorList *osinfo_hypervisorlist_new_union(OsinfoHypervisorList *sourceOne, OsinfoHypervisorList *sourceTwo);

#endif /* __OSINFO_HYPERVISORLIST_H__ */
