/*
 * libosinfo: a virtualization hypervisor
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
#include <osinfo/osinfo_device.h>
#include <osinfo/osinfo_devicelist.h>

#ifndef __OSINFO_HYPERVISOR_H__
#define __OSINFO_HYPERVISOR_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_HYPERVISOR                  (osinfo_hypervisor_get_type ())
#define OSINFO_HYPERVISOR(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_HYPERVISOR, OsinfoHypervisor))
#define OSINFO_IS_HYPERVISOR(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_HYPERVISOR))
#define OSINFO_HYPERVISOR_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_HYPERVISOR, OsinfoHypervisorClass))
#define OSINFO_IS_HYPERVISOR_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_HYPERVISOR))
#define OSINFO_HYPERVISOR_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_HYPERVISOR, OsinfoHypervisorClass))

typedef struct _OsinfoHypervisor        OsinfoHypervisor;

typedef struct _OsinfoHypervisorClass   OsinfoHypervisorClass;

typedef struct _OsinfoHypervisorPrivate OsinfoHypervisorPrivate;

/* object */
struct _OsinfoHypervisor
{
    OsinfoEntity parent_instance;

    /* public */

    /* private */
    OsinfoHypervisorPrivate *priv;
};

/* class */
struct _OsinfoHypervisorClass
{
    OsinfoEntityClass parent_class;

    /* class members */
};

GType osinfo_hypervisor_get_type(void);

OsinfoHypervisor *osinfo_hypervisor_new(const gchar *id);

OsinfoDeviceList *osinfo_hypervisor_get_devices(OsinfoHypervisor *hv, OsinfoFilter *filter);
OsinfoDeviceLinkList *osinfo_hypervisor_get_device_links(OsinfoHypervisor *hv, OsinfoFilter *filter);

OsinfoDeviceLink *osinfo_hypervisor_add_device(OsinfoHypervisor *hv, OsinfoDevice *dev);

#endif /* __OSINFO_HYPERVISOR_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
