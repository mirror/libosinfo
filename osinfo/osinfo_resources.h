/*
 * libosinfo: Required or recommended resources for an (guest) OS
 *
 * Copyright (C) 2009-2011 Red Hat, Inc
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
 *   Zeeshan Ali <zeenix@redhat.com>
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <glib-object.h>
#include <gio/gio.h>
#include <osinfo/osinfo_entity.h>

#ifndef __OSINFO_RESOURCES_H__
#define __OSINFO_RESOURCES_H__

#define OSINFO_ARCHITECTURE_ALL "all"
#define OSINFO_MEGAHERTZ        1000000
#define OSINFO_KIBIBYTES        1024
#define OSINFO_MEBIBYTES        1048576
#define OSINFO_GIBIBYTES        1073741824

/*
 * Type macros.
 */
#define OSINFO_TYPE_RESOURCES                  (osinfo_resources_get_type ())
#define OSINFO_RESOURCES(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                OSINFO_TYPE_RESOURCES, OsinfoResources))
#define OSINFO_IS_RESOURCES(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                                OSINFO_TYPE_RESOURCES))
#define OSINFO_RESOURCES_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                                OSINFO_TYPE_RESOURCES, OsinfoResourcesClass))
#define OSINFO_IS_RESOURCES_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                                                OSINFO_TYPE_RESOURCES))
#define OSINFO_RESOURCES_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                                OSINFO_TYPE_RESOURCES, OsinfoResourcesClass))

typedef struct _OsinfoResources        OsinfoResources;

typedef struct _OsinfoResourcesClass   OsinfoResourcesClass;

typedef struct _OsinfoResourcesPrivate OsinfoResourcesPrivate;

#define OSINFO_RESOURCES_PROP_ARCHITECTURE "architecture"
#define OSINFO_RESOURCES_PROP_CPU          "cpu"
#define OSINFO_RESOURCES_PROP_N_CPUS       "n-cpus"
#define OSINFO_RESOURCES_PROP_RAM          "ram"
#define OSINFO_RESOURCES_PROP_STORAGE      "storage"

/* object */
struct _OsinfoResources
{
    OsinfoEntity parent_instance;

    /* public */

    /* private */
    OsinfoResourcesPrivate *priv;
};

/* class */
struct _OsinfoResourcesClass
{
    OsinfoEntityClass parent_class;

    /* class members */
};

GType osinfo_resources_get_type(void);

OsinfoResources *osinfo_resources_new(const gchar *id, const gchar *architecture);

const gchar *osinfo_resources_get_architecture(OsinfoResources *resources);
gint osinfo_resources_get_n_cpus(OsinfoResources *resources);
gint64 osinfo_resources_get_cpu(OsinfoResources *resources);
gint64 osinfo_resources_get_ram(OsinfoResources *resources);
gint64 osinfo_resources_get_storage(OsinfoResources *resources);

#endif /* __OSINFO_RESOURCES_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
