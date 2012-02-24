/*
 * libosinfo: a operating system deployment for a platform
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
#include <osinfo/osinfo_platform.h>
#include <osinfo/osinfo_os.h>

#ifndef __OSINFO_DEPLOYMENT_H__
#define __OSINFO_DEPLOYMENT_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_DEPLOYMENT                  (osinfo_deployment_get_type ())
#define OSINFO_DEPLOYMENT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_DEPLOYMENT, OsinfoDeployment))
#define OSINFO_IS_DEPLOYMENT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_DEPLOYMENT))
#define OSINFO_DEPLOYMENT_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_DEPLOYMENT, OsinfoDeploymentClass))
#define OSINFO_IS_DEPLOYMENT_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_DEPLOYMENT))
#define OSINFO_DEPLOYMENT_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_DEPLOYMENT, OsinfoDeploymentClass))

typedef struct _OsinfoDeployment        OsinfoDeployment;

typedef struct _OsinfoDeploymentClass   OsinfoDeploymentClass;

typedef struct _OsinfoDeploymentPrivate OsinfoDeploymentPrivate;

/* object */
struct _OsinfoDeployment
{
    OsinfoEntity parent_instance;

    /* public */

    /* private */
    OsinfoDeploymentPrivate *priv;
};

/* class */
struct _OsinfoDeploymentClass
{
    OsinfoEntityClass parent_class;

    /* class members */
};

GType osinfo_deployment_get_type(void);

OsinfoDeployment *osinfo_deployment_new(const gchar *id,
                                        OsinfoOs *os,
                                        OsinfoPlatform *platform);

OsinfoOs *osinfo_deployment_get_os(OsinfoDeployment *deployment);
OsinfoPlatform *osinfo_deployment_get_platform(OsinfoDeployment *deployment);

OsinfoDevice *osinfo_deployment_get_preferred_device(OsinfoDeployment *deployment, OsinfoFilter *filter);
OsinfoDeviceLink *osinfo_deployment_get_preferred_device_link(OsinfoDeployment *deployment, OsinfoFilter *filter);

OsinfoDeviceList *osinfo_deployment_get_devices(OsinfoDeployment *deployment, OsinfoFilter *filter);
OsinfoDeviceLinkList *osinfo_deployment_get_device_links(OsinfoDeployment *deployment, OsinfoFilter *filter);
OsinfoDeviceLink *osinfo_deployment_add_device(OsinfoDeployment *deployment, OsinfoDevice *dev);

#endif /* __OSINFO_DEPLOYMENT_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
