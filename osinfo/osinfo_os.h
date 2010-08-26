/*
 * libosinfo: an operating system
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
#include <osinfo/osinfo_oslist.h>

#ifndef __OSINFO_OS_H__
#define __OSINFO_OS_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_OS                  (osinfo_os_get_type ())
#define OSINFO_OS(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_OS, OsinfoOs))
#define OSINFO_IS_OS(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_OS))
#define OSINFO_OS_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_OS, OsinfoOsClass))
#define OSINFO_IS_OS_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_OS))
#define OSINFO_OS_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_OS, OsinfoOsClass))

typedef struct _OsinfoOs        OsinfoOs;

typedef struct _OsinfoOsClass   OsinfoOsClass;

typedef struct _OsinfoOsPrivate OsinfoOsPrivate;

/* object */
struct _OsinfoOs
{
    OsinfoEntity parent_instance;

    /* public */

    /* private */
    OsinfoOsPrivate *priv;
};

/* class */
struct _OsinfoOsClass
{
    OsinfoEntityClass parent_class;

    /* class members */
};

typedef enum {
  OSINFO_OS_RELATIONSHIP_DERIVES_FROM,
  OSINFO_OS_RELATIONSHIP_UPGRADES,
  OSINFO_OS_RELATIONSHIP_CLONES,
} OsinfoOsRelationship;


GType osinfo_os_get_type(void);

OsinfoOs *osinfo_os_new(const gchar *id);

OsinfoDevice *osinfo_os_get_preferred_device(OsinfoOs *self, OsinfoHypervisor *hv, OsinfoFilter *filter,
					     const gchar **driver);
OsinfoOsList *osinfo_os_get_related(OsinfoOs *self, OsinfoOsRelationship relshp);

OsinfoDeviceList *osinfo_os_get_devices(OsinfoOs *self, OsinfoHypervisor *hv, OsinfoFilter *filter);

void osinfo_os_add_device(OsinfoOs *self, OsinfoHypervisor *hv, OsinfoDevice *dev, const gchar *driver);

void osinfo_os_add_related_os(OsinfoOs *self, OsinfoOsRelationship relshp, OsinfoOs *otheros);

#endif /* __OSINFO_OS_H__ */
