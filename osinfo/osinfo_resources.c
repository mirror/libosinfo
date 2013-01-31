/*
 * libosinfo: Required or recommended resources for an (guest) OS
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
 *   Zeeshan Ali <zeenix@redhat.com>
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <config.h>

#include <osinfo/osinfo.h>
#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>
#include <glib/gi18n-lib.h>

G_DEFINE_TYPE (OsinfoResources, osinfo_resources, OSINFO_TYPE_ENTITY);

enum {
    PROP_0,

    PROP_ARCHITECTURE,
    PROP_CPU,
    PROP_N_CPUS,
    PROP_RAM,
    PROP_STORAGE,
};

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

static void
osinfo_resources_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_resources_parent_class)->finalize (object);
}

static void
osinfo_resources_get_property (GObject    *object,
                               guint       property_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
    OsinfoResources *resources = OSINFO_RESOURCES (object);

    switch (property_id) {
    case PROP_ARCHITECTURE:
        g_value_set_string (value,
                            osinfo_resources_get_architecture (resources));
        break;

    case PROP_N_CPUS:
        g_value_set_int (value,
                         osinfo_resources_get_n_cpus (resources));
        break;

    case PROP_CPU:
        g_value_set_int64 (value,
                           osinfo_resources_get_cpu (resources));
        break;

    case PROP_RAM:
        g_value_set_int64 (value,
                           osinfo_resources_get_ram (resources));
        break;

    case PROP_STORAGE:
        g_value_set_int64 (value,
                           osinfo_resources_get_storage (resources));
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
osinfo_resources_set_property(GObject      *object,
                              guint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
    OsinfoResources *resources = OSINFO_RESOURCES (object);

    switch (property_id) {
    case PROP_ARCHITECTURE:
            osinfo_entity_set_param (OSINFO_ENTITY(resources),
                                     OSINFO_RESOURCES_PROP_ARCHITECTURE,
                                     g_value_get_string (value));
        break;

    case PROP_N_CPUS:
        osinfo_resources_set_n_cpus (resources, g_value_get_int (value));
        break;

    case PROP_CPU:
        osinfo_resources_set_cpu (resources, g_value_get_int64 (value));
        break;

    case PROP_RAM:
        osinfo_resources_set_ram (resources, g_value_get_int64 (value));
        break;

    case PROP_STORAGE:
        osinfo_resources_set_storage (resources, g_value_get_int64 (value));
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

/* Init functions */
static void
osinfo_resources_class_init (OsinfoResourcesClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);
    GParamSpec *pspec;

    g_klass->get_property = osinfo_resources_get_property;
    g_klass->set_property = osinfo_resources_set_property;
    g_klass->finalize = osinfo_resources_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoResourcesPrivate));

    /**
     * OsinfoResources:architecture:
     *
     * The target hardware architecture to which these resources applies.
     */
    pspec = g_param_spec_string ("architecture",
                                 "ARCHITECTURE",
                                 _("CPU Architecture"),
                                 NULL /* default value */,
                                 G_PARAM_READWRITE |
                                 G_PARAM_CONSTRUCT_ONLY |
                                 G_PARAM_STATIC_STRINGS);
    g_object_class_install_property (g_klass,
                                     PROP_ARCHITECTURE,
                                     pspec);

    /**
     * OsinfoResources:cpu:
     *
     * The CPU frequency in hertz (Hz).
     */
    pspec = g_param_spec_int64 ("cpu",
                                "CPU",
                                _("CPU frequency in hertz (Hz)"),
                                -1,
                                G_MAXINT,
                                -1,
                                G_PARAM_READWRITE |
                                G_PARAM_STATIC_STRINGS);
    g_object_class_install_property (g_klass,
                                     PROP_CPU,
                                     pspec);

    /**
     * OsinfoResources:n-cpus:
     *
     * The number of CPUs.
     */
    pspec = g_param_spec_int ("n-cpus",
                              "N-CPUs",
                              _("Number of CPUs"),
                              -1,
                              G_MAXINT,
                              -1,
                              G_PARAM_READWRITE |
                              G_PARAM_STATIC_STRINGS);
    g_object_class_install_property (g_klass,
                                     PROP_N_CPUS,
                                     pspec);

    /**
     * OsinfoResources:ram:
     *
     * The amount of Random Access Memory (RAM) in bytes.
     */
    pspec = g_param_spec_int64 ("ram",
                                "RAM",
                                _("Amount of Random Access Memory (RAM) in bytes"),
                                -1,
                                G_MAXINT64,
                                -1,
                                G_PARAM_READWRITE |
                                G_PARAM_STATIC_STRINGS);
    g_object_class_install_property (g_klass,
                                     PROP_RAM,
                                     pspec);

    /**
     * OsinfoResources:storage:
     *
     * The amount of storage space in bytes.
     */
    pspec = g_param_spec_int64 ("storage",
                                "Storage",
                                _("Amount of storage space in bytes"),
                                -1,
                                G_MAXINT64,
                                -1,
                                G_PARAM_READWRITE |
                                G_PARAM_STATIC_STRINGS);
    g_object_class_install_property (g_klass,
                                     PROP_STORAGE,
                                     pspec);
}

static void
osinfo_resources_init (OsinfoResources *resources)
{
    resources->priv = OSINFO_RESOURCES_GET_PRIVATE(resources);
}

OsinfoResources *osinfo_resources_new(const gchar *id,
                                      const gchar *architecture)
{
    OsinfoResources *resources;

    resources = g_object_new (OSINFO_TYPE_RESOURCES,
                              "id", id,
                              "architecture", architecture,
                              NULL);

    return resources;
}

/**
 * osinfo_resources_get_architecture:
 * @resources: an #OsinfoResources instance
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
 * @resources: an #OsinfoResources instance
 *
 * Retrieves the number of CPUs.
 *
 * Returns: the number of CPUs, or -1.
 */
gint osinfo_resources_get_n_cpus(OsinfoResources *resources)
{
    return (gint) osinfo_entity_get_param_value_int64
            (OSINFO_ENTITY(resources), OSINFO_RESOURCES_PROP_N_CPUS);
}

/**
 * osinfo_resources_get_cpu:
 * @resources: an #OsinfoResources instance
 *
 * Retrieves the CPU frequency in hertz (Hz). Divide the value by #OSINFO_MEGAHERTZ if
 * you need this value in megahertz (MHz).
 *
 * Returns: the CPU frequency, or -1.
 */
gint64 osinfo_resources_get_cpu(OsinfoResources *resources)
{
    return osinfo_entity_get_param_value_int64
            (OSINFO_ENTITY(resources), OSINFO_RESOURCES_PROP_CPU);
}

/**
 * osinfo_resources_get_ram:
 * @resources: an #OsinfoResources instance
 *
 * Retrieves the amount of Random Access Memory (RAM) in bytes. Divide the value
 * by #OSINFO_MEBIBYTES if you need this value in mebibytes.
 *
 * Returns: the amount of RAM, or -1.
 */
gint64 osinfo_resources_get_ram(OsinfoResources *resources)
{
    return osinfo_entity_get_param_value_int64
            (OSINFO_ENTITY(resources), OSINFO_RESOURCES_PROP_RAM);
}

/**
 * osinfo_resources_get_storage:
 * @resources: an #OsinfoResources instance
 *
 * Retrieves the amount of storage space in bytes. Divide the value by
 * #OSINFO_GIBIBYTES if you need this value in gibibytes.
 *
 * Returns: the amount of storage, or -1.
 */
gint64 osinfo_resources_get_storage(OsinfoResources *resources)
{
    return osinfo_entity_get_param_value_int64
            (OSINFO_ENTITY(resources), OSINFO_RESOURCES_PROP_STORAGE);
}

/**
 * osinfo_resources_set_n_cpus:
 * @resources: an #OsinfoResources instance
 * @n_cpus: the number of CPUs
 *
 * Sets the number of CPUs.
 */
void osinfo_resources_set_n_cpus(OsinfoResources *resources, gint n_cpus)
{
    osinfo_entity_set_param_int64(OSINFO_ENTITY(resources),
                                  OSINFO_RESOURCES_PROP_N_CPUS,
                                  n_cpus);
}

/**
 * osinfo_resources_set_cpu:
 * @resources: an #OsinfoResources instance
 * @cpu: the CPU frequency in hertz (Hz)
 *
 * Sets the CPU frequency.
 */
void osinfo_resources_set_cpu(OsinfoResources *resources, gint64 cpu)
{
    osinfo_entity_set_param_int64(OSINFO_ENTITY(resources),
                                  OSINFO_RESOURCES_PROP_CPU,
                                  cpu);
}

/**
 * osinfo_resources_set_ram:
 * @resources: an #OsinfoResources instance
 * @ram: the amount of ram in bytes
 *
 * Sets the amount of RAM in bytes.
 */
void osinfo_resources_set_ram(OsinfoResources *resources, gint64 ram)
{
    osinfo_entity_set_param_int64(OSINFO_ENTITY(resources),
                                  OSINFO_RESOURCES_PROP_RAM,
                                  ram);
}

/**
 * osinfo_resources_set_storage:
 * @resources: an #OsinfoResources instance
 * @storage: the amount of storage in bytes
 *
 * Sets the amount of storage space.
 */
void osinfo_resources_set_storage(OsinfoResources *resources, gint64 storage)
{
    osinfo_entity_set_param_int64(OSINFO_ENTITY(resources),
                                  OSINFO_RESOURCES_PROP_STORAGE,
                                  storage);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
