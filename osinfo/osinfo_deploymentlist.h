/*
 * libosinfo: a list of deployments
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

#include <glib-object.h>
#include <osinfo/osinfo_filter.h>
#include <osinfo/osinfo_list.h>

#ifndef __OSINFO_DEPLOYMENTLIST_H__
#define __OSINFO_DEPLOYMENTLIST_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_DEPLOYMENTLIST                  (osinfo_deploymentlist_get_type ())
#define OSINFO_DEPLOYMENTLIST(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_DEPLOYMENTLIST, OsinfoDeploymentList))
#define OSINFO_IS_DEPLOYMENTLIST(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_DEPLOYMENTLIST))
#define OSINFO_DEPLOYMENTLIST_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_DEPLOYMENTLIST, OsinfoDeploymentListClass))
#define OSINFO_IS_DEPLOYMENTLIST_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_DEPLOYMENTLIST))
#define OSINFO_DEPLOYMENTLIST_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_DEPLOYMENTLIST, OsinfoDeploymentListClass))

typedef struct _OsinfoDeploymentList        OsinfoDeploymentList;

typedef struct _OsinfoDeploymentListClass   OsinfoDeploymentListClass;

typedef struct _OsinfoDeploymentListPrivate OsinfoDeploymentListPrivate;

/* object */
struct _OsinfoDeploymentList
{
    OsinfoList parent_instance;

    /* public */

    /* private */
    OsinfoDeploymentListPrivate *priv;
};

/* class */
struct _OsinfoDeploymentListClass
{
    /*< private >*/
    OsinfoListClass parent_class;

    /* class members */
};

GType osinfo_deploymentlist_get_type(void);

OsinfoDeploymentList *osinfo_deploymentlist_new(void);
OsinfoDeploymentList *osinfo_deploymentlist_new_copy(OsinfoDeploymentList *source);
OsinfoDeploymentList *osinfo_deploymentlist_new_filtered(OsinfoDeploymentList *source, OsinfoFilter *filter);
OsinfoDeploymentList *osinfo_deploymentlist_new_intersection(OsinfoDeploymentList *sourceOne, OsinfoDeploymentList *sourceTwo);
OsinfoDeploymentList *osinfo_deploymentlist_new_union(OsinfoDeploymentList *sourceOne, OsinfoDeploymentList *sourceTwo);

#endif /* __OSINFO_DEPLOYMENTLIST_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
