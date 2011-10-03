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

#include <osinfo/osinfo.h>
#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>

G_DEFINE_TYPE (OsinfoResources, osinfo_resources, OSINFO_TYPE_ENTITY);

#define OSINFO_RESOURCES_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                      OSINFO_TYPE_RESOURCES, \
                                      OsinfoResourcesPrivate))

/**
 * SECTION:osinfo_resources
 * @short_description: Required or recommended resources for an (guest) OS
 * @see_also: #OsinfoOs
 *
 * #OsinfoResources is an entity representing required or recommended resources
 * for an (guest) operating system.
 */

struct _OsinfoResourcesPrivate
{
    gboolean unused;
};

static gint64 get_param_as_int64(OsinfoResources *resources,
                                 const gchar *key)
{
    const gchar *str;

    str = osinfo_entity_get_param_value(OSINFO_ENTITY(resources), key);

    if (str == NULL)
        return -1;

    return (gint64) g_ascii_strtod(str, NULL);
}

static void set_param_from_int64(OsinfoResources *resources,
                                 const gchar *key,
                                 gint64 value)
{
    gchar *str;

    str = g_strdup_printf("%"G_GUINT64_FORMAT, value);
    osinfo_entity_set_param(OSINFO_ENTITY(resources), key, str);
}

static void
osinfo_resources_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_resources_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_resources_class_init (OsinfoResourcesClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_resources_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoResourcesPrivate));
}

static void
osinfo_resources_init (OsinfoResources *resources)
{
    OsinfoResourcesPrivate *priv;
    resources->priv = priv = OSINFO_RESOURCES_GET_PRIVATE(resources);
}

OsinfoResources *osinfo_resources_new(const gchar *id,
                                      const gchar *architecture)
{
    OsinfoResources *resources;

    resources = g_object_new(OSINFO_TYPE_RESOURCES,
                             "id", id,
                             NULL);

    if (architecture != NULL)
        osinfo_entity_set_param(OSINFO_ENTITY(resources),
                                OSINFO_RESOURCES_PROP_ARCHITECTURE,
                                architecture);

    return resources;
}

/**
 * osinfo_resources_get_architecture:
 * @resources: a #OsinfoResources instance
 *
 * Retrieves the target hardware architecture to which @resources applies. Some
 * operating systems specify the same requirements and recommendations for all
 * architectures. In such cases, the string returned by this call will be
 * #OSINFO_ARCHITECTURE_ALL.
 *
 * Returns: (transfer none): the hardware architecture.
 */
const gchar *osinfo_resources_get_architecture(OsinfoResources *resources)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(resources),
                                         OSINFO_RESOURCES_PROP_ARCHITECTURE);
}

/**
 * osinfo_resources_get_n_cpus:
 * @resources: a #OsinfoResources instance
 *
 * Retrieves the number of CPUs.
 *
 * Returns: the number of CPUs, or -1.
 */
gint osinfo_resources_get_n_cpus(OsinfoResources *resources)
{
    return (gint) get_param_as_int64(resources, OSINFO_RESOURCES_PROP_N_CPUS);
}

/**
 * osinfo_resources_get_cpu:
 * @resources: a #OsinfoResources instance
 *
 * Retrieves the CPU frequency in hertz (Hz). Divide the value by #OSINFO_MEGAHERTZ if
 * you need this value in megahertz (MHz).
 *
 * Returns: the CPU frequency, or -1.
 */
gint64 osinfo_resources_get_cpu(OsinfoResources *resources)
{
    return get_param_as_int64(resources, OSINFO_RESOURCES_PROP_CPU);
}

/**
 * osinfo_resources_get_ram:
 * @resources: a #OsinfoResources instance
 *
 * Retrieves the amount of Random Access Memory (RAM) in bytes. Divide the value
 * by #OSINFO_MEBIBYTES if you need this value in mebibytes.
 *
 * Returns: the amount of RAM, or -1.
 */
gint64 osinfo_resources_get_ram(OsinfoResources *resources)
{
    return get_param_as_int64(resources, OSINFO_RESOURCES_PROP_RAM);
}

/**
 * osinfo_resources_get_storage:
 * @resources: a #OsinfoResources instance
 *
 * Retrieves the amount of storage space in bytes. Divide the value by
 * #OSINFO_GIBIBYTES if you need this value in gibibytes.
 *
 * Returns: the amount of storage, or -1.
 */
gint64 osinfo_resources_get_storage(OsinfoResources *resources)
{
    return get_param_as_int64(resources, OSINFO_RESOURCES_PROP_STORAGE);
}

/**
 * osinfo_resources_set_n_cpus:
 * @resources: a #OsinfoResources instance
 * @n_cpus: the number of CPUs
 *
 * Sets the number of CPUs.
 */
void osinfo_resources_set_n_cpus(OsinfoResources *resources, gint n_cpus)
{
    set_param_from_int64 (resources, OSINFO_RESOURCES_PROP_N_CPUS, n_cpus);
}

/**
 * osinfo_resources_set_cpu:
 * @resources: a #OsinfoResources instance
 * @cpu: the CPU frequency in hertz (Hz)
 *
 * Sets the CPU frequency.
 */
void osinfo_resources_set_cpu(OsinfoResources *resources, gint64 cpu)
{
    set_param_from_int64 (resources, OSINFO_RESOURCES_PROP_CPU, cpu);
}

/**
 * osinfo_resources_set_ram:
 * @resources: a #OsinfoResources instance
 * @ram: the amount of ram in bytes
 *
 * Sets the amount of RAM in bytes.
 */
void osinfo_resources_set_ram(OsinfoResources *resources, gint64 ram)
{
    set_param_from_int64 (resources, OSINFO_RESOURCES_PROP_RAM, ram);
}

/**
 * osinfo_resources_set_storage:
 * @resources: a #OsinfoResources instance
 * @storage: the amount of storage in bytes
 *
 * Sets the amount of storage space.
 */
void osinfo_resources_set_storage(OsinfoResources *resources, gint64 storage)
{
    set_param_from_int64 (resources, OSINFO_RESOURCES_PROP_STORAGE, storage);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
