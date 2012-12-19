/*
 * libosinfo:
 *
 * Copyright (C) 2009-2012 Red Hat, Inc.
 * Copyright (C) 2012 Fabiano Fidêncio
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
 *   Fabiano Fidêncio <fabiano@fidencio.org>
 */

#include <config.h>

#include <osinfo/osinfo.h>
#include <glib/gi18n-lib.h>

G_DEFINE_TYPE (OsinfoInstallConfigParam, osinfo_install_config_param, OSINFO_TYPE_ENTITY);

#define OSINFO_INSTALL_CONFIG_PARAM_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_INSTALL_CONFIG_PARAM, OsinfoInstallConfigParamPrivate))

/**
 * SECTION:osinfo_install_config_param
 * @short_description: OS install configuration parameters (and its policies)
 * @see_also: #OsinfoInstallScript, #OsinfoInstallSciptConfig
 *
 * #OsinfoInstallConfigParam is an entity for representing all parameters that
 * can be set in an automated installation. It is used to help applications to
 * generate an automated installation script
 */

enum {
    PROP_0,

    PROP_NAME,
    PROP_POLICY,
};

static void
osinfo_install_config_param_set_property(GObject *object,
                                         guint property_id,
                                         const GValue *value,
                                         GParamSpec *pspec)
{
    OsinfoInstallConfigParam *config_param =
        OSINFO_INSTALL_CONFIG_PARAM (object);

    switch (property_id) {
    case PROP_NAME:
        osinfo_entity_set_param(OSINFO_ENTITY(config_param),
                                OSINFO_INSTALL_CONFIG_PARAM_PROP_NAME,
                                g_value_get_string(value));
        break;
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
osinfo_install_config_param_get_property(GObject *object,
                                         guint property_id,
                                         GValue *value,
                                         GParamSpec *pspec)
{
    OsinfoInstallConfigParam *config_param =
        OSINFO_INSTALL_CONFIG_PARAM (object);

    switch (property_id) {
    case PROP_NAME:
    {
        const gchar *name;

        name = osinfo_install_config_param_get_name(config_param);
        g_value_set_string(value, name);
        break;
    }
    case PROP_POLICY:
    {
        OsinfoInstallConfigParamPolicy policy;

        policy = osinfo_install_config_param_get_policy(config_param);
        g_value_set_enum(value, policy);
        break;
    }
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

/* Init functions */
static void
osinfo_install_config_param_class_init (OsinfoInstallConfigParamClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);
    GParamSpec *pspec;

    g_klass->set_property = osinfo_install_config_param_set_property;
    g_klass->get_property = osinfo_install_config_param_get_property;

    /**
     * OsinfoInstallConfigParam:name:
     *
     * The name of the configuration parameter.
     **/
    pspec = g_param_spec_string("name",
                                "Name",
                                _("Parameter name"),
                                NULL,
                                G_PARAM_WRITABLE |
                                G_PARAM_READABLE |
                                G_PARAM_CONSTRUCT_ONLY |
                                G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(g_klass,
                                    PROP_NAME,
                                    pspec);
    /**
     * OsinfoInstallConfigParam:policy:
     *
     * The policy of the configuration parameter.
     **/
    pspec = g_param_spec_enum("policy",
                              "Policy",
                              _("Parameter policy"),
                              OSINFO_TYPE_INSTALL_CONFIG_PARAM_POLICY,
                              OSINFO_INSTALL_CONFIG_PARAM_POLICY_OPTIONAL,
                              G_PARAM_READABLE |
                              G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(g_klass,
                                    PROP_POLICY,
                                    pspec);
}

static void
osinfo_install_config_param_init (OsinfoInstallConfigParam *config_param)
{
   /* G_DEFINE_TYPE() needs an instance init function */
}

/**
 * osinfo_install_config_param_new:
 * @name: the configuration parameter name
 *
 * Construct a new configuration parameter for an #OsinfoInstallScript.
 *
 * Returns: (transfer full): the new configuration parameter
 */
OsinfoInstallConfigParam *osinfo_install_config_param_new(const gchar *name)
{
    return g_object_new(OSINFO_TYPE_INSTALL_CONFIG_PARAM, "name", name, NULL);
}

/**
 * osinfo_install_config_param_get_name:
 * @config_param: the configuration parameter
 *
 * Returns: (transfer none): the name of the configuration parameter
 */
const gchar *osinfo_install_config_param_get_name(const OsinfoInstallConfigParam *config_param)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config_param),
                                         OSINFO_INSTALL_CONFIG_PARAM_PROP_NAME);
}

/**
 * osinfo_install_config_param_get_policy:
 * @config_param: the configuration parameter
 *
 * Returns: (transfer none): the policy of the configuration parameter
 */
OsinfoInstallConfigParamPolicy osinfo_install_config_param_get_policy(const OsinfoInstallConfigParam *config_param)
{
    return osinfo_entity_get_param_value_enum(OSINFO_ENTITY(config_param),
                                              OSINFO_INSTALL_CONFIG_PARAM_PROP_POLICY,
                                              OSINFO_TYPE_INSTALL_CONFIG_PARAM_POLICY,
                                              OSINFO_INSTALL_CONFIG_PARAM_POLICY_OPTIONAL);
}

/**
 * osinfo_install_config_param_is_required:
 * @config_param: the configuration parameter
 *
 * Returns: (transfer none): TRUE if the config_param is required.
 *                           FALSE otherwise.
 */
gboolean osinfo_install_config_param_is_required(const OsinfoInstallConfigParam *config_param)
{
    return (osinfo_install_config_param_get_policy(config_param) ==
            OSINFO_INSTALL_CONFIG_PARAM_POLICY_REQUIRED);
}

/**
 * osinfo_install_config_param_is_optional:
 * @config_param: the configuration parameter
 *
 * Returns: (transfer none): TRUE if the config_param is optional.
 *                           FALSE otherwise.
 */
gboolean osinfo_install_config_param_is_optional(const OsinfoInstallConfigParam *config_param)
{
    return (osinfo_install_config_param_get_policy(config_param) ==
            OSINFO_INSTALL_CONFIG_PARAM_POLICY_OPTIONAL);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
