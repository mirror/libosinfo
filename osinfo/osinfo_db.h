/*
 * libosinfo: Main information database
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
#include <osinfo/osinfo_hypervisor.h>
#include <osinfo/osinfo_os.h>
#include <osinfo/osinfo_device.h>
#include <osinfo/osinfo_hypervisorlist.h>
#include <osinfo/osinfo_oslist.h>
#include <osinfo/osinfo_devicelist.h>

#ifndef __OSINFO_DB_H__
#define __OSINFO_DB_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_DB                  (osinfo_db_get_type ())
#define OSINFO_DB(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_DB, OsinfoDb))
#define OSINFO_IS_DB(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_DB))
#define OSINFO_DB_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_DB, OsinfoDbClass))
#define OSINFO_IS_DB_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_DB))
#define OSINFO_DB_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_DB, OsinfoDbClass))

typedef struct _OsinfoDb        OsinfoDb;

typedef struct _OsinfoDbClass   OsinfoDbClass;

typedef struct _OsinfoDbPrivate OsinfoDbPrivate;

/*
 * To get a db handle, we construct one with a construct-time only
 * backing data directory. It is already considered to be initialized
 * on return from the constructor, and ready to do work.
 *
 * To close it, we call the destructor on it.
 * Setting parameters on it will work if it's not a construct-time only
 * parameter. Reading will always work. Currently the backing directory and
 * libvirt version are the only parameters.
 *
 * The db object contains information related to three main classes of
 * objects: hypervisors, operating systems and devices.
 */

/* object */
struct _OsinfoDb
{
    GObject parent_instance;

    /* public */

    /* private */
    OsinfoDbPrivate *priv;
};

/* class */
struct _OsinfoDbClass
{
    GObjectClass parent_class;

    /* class members */
};

GType osinfo_db_get_type(void);

OsinfoDb *osinfo_db_new(void);

OsinfoHypervisor *osinfo_db_get_hypervisor(OsinfoDb *self, const gchar *hvId);
OsinfoDevice *osinfo_db_get_device(OsinfoDb *self, const gchar *devId);
OsinfoOs *osinfo_db_get_os(OsinfoDb *self, const gchar *osId);

OsinfoOsList *osinfo_db_get_os_list(OsinfoDb *self);
OsinfoHypervisorList *osinfo_db_get_hypervisor_list(OsinfoDb *self);
OsinfoDeviceList *osinfo_db_get_device_list(OsinfoDb *self);

void osinfo_db_add_os(OsinfoDb *self, OsinfoOs *os);
void osinfo_db_add_hypervisor(OsinfoDb *self, OsinfoHypervisor *hv);
void osinfo_db_add_device(OsinfoDb *self, OsinfoDevice *device);

// Get me all unique values for property "vendor" among operating systems
GList *osinfo_db_unique_values_for_property_in_os(OsinfoDb *self, const gchar *propName);

// Get me all unique values for property "vendor" among hypervisors
GList *osinfo_db_unique_values_for_property_in_hv(OsinfoDb *self, const gchar *propName);

// Get me all unique values for property "vendor" among devices
GList *osinfo_db_unique_values_for_property_in_dev(OsinfoDb *self, const gchar *propName);

// Get me all OSes that 'upgrade' another OS (or whatever relationship is specified)
OsinfoOsList *osinfo_db_unique_values_for_os_relationship(OsinfoDb *self, OsinfoOsRelationship relshp);


#endif /* __OSINFO_DB_H__ */
