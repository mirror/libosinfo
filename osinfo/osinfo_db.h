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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Authors:
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <glib-object.h>
#include <gio/gio.h>
#include <osinfo/osinfo_platform.h>
#include <osinfo/osinfo_os.h>
#include <osinfo/osinfo_device.h>
#include <osinfo/osinfo_platformlist.h>
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

OsinfoPlatform *osinfo_db_get_platform(OsinfoDb *db, const gchar *id);
OsinfoDevice *osinfo_db_get_device(OsinfoDb *db, const gchar *id);
OsinfoOs *osinfo_db_get_os(OsinfoDb *db, const gchar *id);
OsinfoDeployment *osinfo_db_get_deployment(OsinfoDb *db, const gchar *id);

OsinfoDeployment *osinfo_db_find_deployment(OsinfoDb *db,
                                            OsinfoOs *os,
                                            OsinfoPlatform *platform);

OsinfoOsList *osinfo_db_get_os_list(OsinfoDb *db);
OsinfoPlatformList *osinfo_db_get_platform_list(OsinfoDb *db);
OsinfoDeviceList *osinfo_db_get_device_list(OsinfoDb *db);
OsinfoDeploymentList *osinfo_db_get_deployment_list(OsinfoDb *db);

void osinfo_db_add_os(OsinfoDb *db, OsinfoOs *os);
void osinfo_db_add_platform(OsinfoDb *db, OsinfoPlatform *platform);
void osinfo_db_add_device(OsinfoDb *db, OsinfoDevice *device);
void osinfo_db_add_deployment(OsinfoDb *db, OsinfoDeployment *deployment);

OsinfoOs *osinfo_db_guess_os_from_media(OsinfoDb *db, OsinfoMedia *media);

// Get me all unique values for property "vendor" among operating systems
GList *osinfo_db_unique_values_for_property_in_os(OsinfoDb *db, const gchar *propName);

// Get me all unique values for property "vendor" among platforms
GList *osinfo_db_unique_values_for_property_in_platform(OsinfoDb *db, const gchar *propName);

// Get me all unique values for property "vendor" among devices
GList *osinfo_db_unique_values_for_property_in_device(OsinfoDb *db, const gchar *propName);

// Get me all unique values for property "vendor" among deployments
GList *osinfo_db_unique_values_for_property_in_deployment(OsinfoDb *db, const gchar *propName);

// Get me all OSes that 'upgrade' another OS (or whatever relationship is specified)
OsinfoOsList *osinfo_db_unique_values_for_os_relationship(OsinfoDb *db, OsinfoProductRelationship relshp);

// Get me all Platforms that 'upgrade' another Platform (or whatever relationship is specified)
OsinfoPlatformList *osinfo_db_unique_values_for_platform_relationship(OsinfoDb *db, OsinfoProductRelationship relshp);


#endif /* __OSINFO_DB_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
