/*
 * libosinfo:
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
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <config.h>

#include <osinfo/osinfo.h>
#include "osinfo/osinfo_install_config_private.h"
#include <glib/gi18n-lib.h>

G_DEFINE_TYPE (OsinfoInstallConfig, osinfo_install_config, OSINFO_TYPE_ENTITY);

#define OSINFO_INSTALL_CONFIG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_INSTALL_CONFIG, OsinfoInstallConfigPrivate))

/**
 * SECTION:osinfo_install_config
 * @short_description: OS install configuration
 * @see_also: #OsinfoInstallScript
 *
 * #OsinfoInstallConfig is an object for representing OS
 * install configuration data. It is used to generate an
 * automated installation script
 */

struct _OsinfoInstallConfigPrivate
{
    OsinfoInstallConfigParamList *config_params;
};

enum {
    PROP_0,

    PROP_CONFIG_PARAMS,
};

static void
osinfo_install_config_set_property(GObject    *object,
                                   guint       property_id,
                                   const GValue     *value,
                                   GParamSpec *pspec)
{
    OsinfoInstallConfig *config = OSINFO_INSTALL_CONFIG(object);

    switch (property_id) {
    case PROP_CONFIG_PARAMS:
        osinfo_install_config_set_config_params(config, g_value_get_object(value));
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
osinfo_install_config_get_property(GObject    *object,
                                   guint       property_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
    OsinfoInstallConfig *config = OSINFO_INSTALL_CONFIG(object);

    switch (property_id) {
    case PROP_CONFIG_PARAMS:
        g_value_set_object(value, osinfo_install_config_get_config_params(config));
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
osinfo_install_config_finalize (GObject *object)
{
    OsinfoInstallConfig *config = OSINFO_INSTALL_CONFIG (object);

    if (config->priv->config_params)
        g_object_unref(config->priv->config_params);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_install_config_parent_class)->finalize (object);
}


/* Init functions */
static void
osinfo_install_config_class_init (OsinfoInstallConfigClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);
    GParamSpec *pspec;

    g_klass->get_property = osinfo_install_config_get_property;
    g_klass->set_property = osinfo_install_config_set_property;
    g_klass->finalize = osinfo_install_config_finalize;

    pspec = g_param_spec_object("config-params",
                                "Config Parameters",
                                _("Valid configuration parameters"),
                                OSINFO_TYPE_INSTALL_CONFIG_PARAMLIST,
                                G_PARAM_READWRITE |
                                G_PARAM_CONSTRUCT_ONLY |
                                G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(g_klass,
                                    PROP_CONFIG_PARAMS,
                                    pspec);

    g_type_class_add_private (klass, sizeof (OsinfoInstallConfigPrivate));
}

static const gchar valid[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'X', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_', '+',
    '=', '!', '@', '#', '%', '^', '&', ',', '(', ')', '[', '{', '}',
    '[', ']', ';', ':', '<', '>', ',', '.', '?', '/', '~',
};


static void
osinfo_install_config_init (OsinfoInstallConfig *config)
{
    OsinfoInstallConfigPrivate *priv;
    gchar pass[9];
    gsize i;

    config->priv = priv = OSINFO_INSTALL_CONFIG_GET_PRIVATE(config);

    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_L10N_KEYBOARD,
                            "en_US.UTF-8");
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_L10N_TIMEZONE,
                            "America/New_York");
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_L10N_LANGUAGE,
                            "en_US.UTF-8");

    for (i = 0 ; i < sizeof(pass)-1 ; i++) {
        gint val = g_random_int_range(0, sizeof(valid));
        pass[i] = valid[val];
    }
    pass[sizeof(pass)-1] = '\0';

    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_ADMIN_PASSWORD,
                            pass);
}


/**
 * osinfo_install_config_new:
 * @id: the unique identifier
 *
 * Construct a new install configuration with default values for
 * language, keyboard, timezone and admin password. The default values
 * are to use an 'en_US.UTF-8' language and keyboard, and an
 * 'America/New_York' timezone. The admin password is set to a random
 * 8 character password.
 *
 * Returns: (transfer full): an install configuration with default
 * values
 *
 */
OsinfoInstallConfig *osinfo_install_config_new(const gchar *id)
{
    g_return_val_if_fail(id != NULL, NULL);

    return g_object_new(OSINFO_TYPE_INSTALL_CONFIG, "id", id, NULL);
}


/**
 * osinfo_install_config_new_for_script:
 * @id: the unique identifier
 * @script: the #OsinfoInstallScript we are creating the configuration for
 *
 * Construct a new install configuration associated with @script.
 * OsinfoInstallConfig:config-params will contain the
 * #OsinfoInstallConfigParamList describing the parameters that can be set
 * on the config object when creating a configuration for @script. See
 * osinfo_install_config_new() for a description of the default values that
 * will be set on the newly created #OsinfoInstallConfig.
 *
 * Returns: (transfer full): an install configuration
 */
OsinfoInstallConfig *osinfo_install_config_new_for_script(const gchar *id,
                                                          OsinfoInstallScript *script)
{
    OsinfoInstallConfigParamList *params;

    g_return_val_if_fail(id != NULL, NULL);
    g_return_val_if_fail(script != NULL, NULL);

    params = osinfo_install_script_get_config_params(script);

    return g_object_new(OSINFO_TYPE_INSTALL_CONFIG,
                        "id", id,
                        "config-params", params,
                        NULL);
}

void osinfo_install_config_set_hardware_arch(OsinfoInstallConfig *config,
                                             const gchar *arch)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_HARDWARE_ARCH,
                            arch);
}


const gchar *osinfo_install_config_get_hardware_arch(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_HARDWARE_ARCH);
}


/**
 * osinfo_install_config_set_l10n_keyboard:
 * @config: the install config
 * @keyboard: the keyboard
 *
 * Sets the #OSINFO_INSTALL_CONFIG_PROP_L10N_KEYBOARD parameter.
 *
 * The expected format of this string is the same as
 * #osinfo_install_config_set_l10n_language function's 'language' parameter.
 */
void osinfo_install_config_set_l10n_keyboard(OsinfoInstallConfig *config,
                                             const gchar *keyboard)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_L10N_KEYBOARD,
                            keyboard);
}


const gchar *osinfo_install_config_get_l10n_keyboard(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_L10N_KEYBOARD);
}

/**
 * osinfo_install_config_set_l10n_language:
 * @config: the install config
 * @language: the language
 *
 * Sets the #OSINFO_INSTALL_CONFIG_PROP_L10N_LANGUAGE parameter.
 *
 * The expected format of this string is the gettext locale names standard:
 *
 * https://www.gnu.org/savannah-checkouts/gnu/gettext/manual/html_node/Locale-Names.html
 *
 * Both encoding and variant are accepted but optional. For example, both 'pt_BR'
 * and 'pt_BR.utf8' are accepted as the language codes for Brazilian Portuguese.
 */
void osinfo_install_config_set_l10n_language(OsinfoInstallConfig *config,
                                             const gchar *language)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_L10N_LANGUAGE,
                            language);
}


const gchar *osinfo_install_config_get_l10n_language(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_L10N_LANGUAGE);
}


void osinfo_install_config_set_l10n_timezone(OsinfoInstallConfig *config,
                                             const gchar *tz)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_L10N_TIMEZONE,
                            tz);
}


const gchar *osinfo_install_config_get_l10n_timezone(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_L10N_TIMEZONE);
}


void osinfo_install_config_set_admin_password(OsinfoInstallConfig *config,
                                              const gchar *password)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_ADMIN_PASSWORD,
                            password);
}


const gchar *osinfo_install_config_get_admin_password(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_ADMIN_PASSWORD);
}


void osinfo_install_config_set_user_login(OsinfoInstallConfig *config,
                                          const gchar *username)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_USER_LOGIN,
                            username);
}


const gchar *osinfo_install_config_get_user_login(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_USER_LOGIN);
}



void osinfo_install_config_set_user_password(OsinfoInstallConfig *config,
                                             const gchar *password)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_USER_PASSWORD,
                            password);
}


const gchar *osinfo_install_config_get_user_password(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_USER_PASSWORD);
}


void osinfo_install_config_set_user_realname(OsinfoInstallConfig *config,
                                             const gchar *name)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_USER_REALNAME,
                            name);
}


const gchar *osinfo_install_config_get_user_realname(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_USER_REALNAME);
}



void osinfo_install_config_set_user_autologin(OsinfoInstallConfig *config,
                                              gboolean autologin)
{
    osinfo_entity_set_param_boolean(OSINFO_ENTITY(config),
                                    OSINFO_INSTALL_CONFIG_PROP_USER_AUTOLOGIN,
                                    autologin);
}


gboolean osinfo_install_config_get_user_autologin(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value_boolean(OSINFO_ENTITY(config),
                                                 OSINFO_INSTALL_CONFIG_PROP_USER_AUTOLOGIN);
}


void osinfo_install_config_set_user_administrator(OsinfoInstallConfig *config,
                                                  gboolean admin)
{
    osinfo_entity_set_param_boolean(OSINFO_ENTITY(config),
                                    OSINFO_INSTALL_CONFIG_PROP_USER_ADMIN,
                                    admin);
}


gboolean osinfo_install_config_get_user_administrator(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value_boolean(OSINFO_ENTITY(config),
                                                 OSINFO_INSTALL_CONFIG_PROP_USER_ADMIN);
}


void osinfo_install_config_set_reg_login(OsinfoInstallConfig *config,
                                         const gchar *name)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_REG_LOGIN,
                            name);
}

const gchar *osinfo_install_config_get_reg_login(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_REG_LOGIN);
}


void osinfo_install_config_set_reg_password(OsinfoInstallConfig *config,
                                            const gchar *password)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_REG_PASSWORD,
                            password);
}

const gchar *osinfo_install_config_get_reg_password(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_REG_PASSWORD);
}


void osinfo_install_config_set_reg_product_key(OsinfoInstallConfig *config,
                                               const gchar *key)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_REG_PRODUCTKEY,
                            key);
}

const gchar *osinfo_install_config_get_reg_product_key(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_REG_PRODUCTKEY);
}

void osinfo_install_config_set_hostname(OsinfoInstallConfig *config,
                                        const gchar *hostname)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_HOSTNAME,
                            hostname);
}

const gchar *osinfo_install_config_get_hostname(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_HOSTNAME);
}

/**
 * osinfo_install_config_set_target_disk:
 * @config: the install config
 * @disk: the target disk
 *
 * Sets the #OSINFO_INSTALL_CONFIG_PROP_TARGET_DISK parameter.
 *
 * Note that the format of this string is dependent on the installer script
 * @config is going to be used with. You can use
 * #osinfo_install_script_get_path_format() to find out which format
 * does the script expects this string to be in. In case of
 * #OSINFO_PATH_FORMAT_UNIX unix device node names are expected, e.g "/dev/fd0".
 * In case of #OSINFO_PATH_FORMAT_DOS drive letters are expected, e.g "A".
 */
void osinfo_install_config_set_target_disk(OsinfoInstallConfig *config,
                                           const gchar *disk)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_TARGET_DISK,
                            disk);
}

/**
 * osinfo_install_config_get_target_disk:
 * @config: the install config
 *
 * Returns: The value of #OSINFO_INSTALL_CONFIG_PROP_TARGET_DISK parameter,
 *          or NULL.
 */
const gchar *osinfo_install_config_get_target_disk(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_TARGET_DISK);
}

/**
 * osinfo_install_config_set_script_disk:
 * @config: the install config
 * @disk: the disk
 *
 * Sets the #OSINFO_INSTALL_CONFIG_PROP_SCRIPT_DISK parameter.
 *
 * Please read documentation on #osinfo_install_config_set_target_disk() for
 * explanation on the format of @disk string.
 */
void osinfo_install_config_set_script_disk(OsinfoInstallConfig *config,
                                           const gchar *disk)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_SCRIPT_DISK,
                            disk);
}

/**
 * osinfo_install_config_get_script_disk:
 * @config: the install config
 *
 * Returns: The value of #OSINFO_INSTALL_CONFIG_PROP_SCRIPT_DISK parameter,
 *          or NULL.
 */
const gchar *osinfo_install_config_get_script_disk(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_SCRIPT_DISK);
}

/**
 * osinfo_install_config_set_avatar_location:
 * @config: the install config
 * @location: new location
 *
 * Sets the #OSINFO_INSTALL_CONFIG_PROP_AVATAR_LOCATION parameter.
 *
 * Note that the format of this string is dependent on the installer script
 * @config is going to be used with. You can use
 * #osinfo_install_script_get_path_format() to find out which format
 * does the script expects this string to be in.
 *
 * Also note that in case of #OSINFO_PATH_FORMAT_DOS, the drive/disk letter
 * and the leading ':' must not be included in the path.
 */
void osinfo_install_config_set_avatar_location(OsinfoInstallConfig *config,
                                               const gchar *location)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_AVATAR_LOCATION,
                            location);
}

/**
 * osinfo_install_config_get_avatar_location:
 * @config: the install config
 *
 * Returns: The value of #OSINFO_INSTALL_CONFIG_PROP_AVATAR_LOCATION parameter,
 *          or NULL.
 */
const gchar *osinfo_install_config_get_avatar_location(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_AVATAR_LOCATION);
}

/**
 * osinfo_install_config_set_avatar_disk:
 * @config: the install config
 * @disk: the avatar disk
 *
 * Sets the #OSINFO_INSTALL_CONFIG_PROP_AVATAR_DISK parameter.
 *
 * Please read documentation on #osinfo_install_config_set_target_disk() for
 * explanation on the format of @disk string.
 */
void osinfo_install_config_set_avatar_disk(OsinfoInstallConfig *config,
                                           const gchar *disk)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_AVATAR_DISK,
                            disk);
}

/**
 * osinfo_install_config_get_avatar_disk:
 * @config: the install config
 *
 * Returns: The value of #OSINFO_INSTALL_CONFIG_PROP_AVATAR_DISK parameter,
 *          or NULL.
 */
const gchar *osinfo_install_config_get_avatar_disk(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(config),
                                         OSINFO_INSTALL_CONFIG_PROP_AVATAR_DISK);
}

/**
 * osinfo_install_config_set_pre_install_drivers_disk:
 * @config: the install config
 * @disk: the disk
 *
 * Specify the disk on which drivers to be installed at the very beginning of
 * installation, are available. This is usually needed for devices for which the
 * OS in question does not have out of the box support for and devices are
 * required/prefered to be available during actual installation.
 *
 * Please read documentation on #osinfo_install_config_set_target_disk() for
 * explanation on the format of @disk string.
 *
 * NOTE: Not every install script supports pre-installation of drivers. Use
 * #osinfo_install_script_get_can_pre_install_drivers() to find out if an
 * installer script supports it.
 *
 * NOTE: Microsoft Windows XP requires pre-installation driver files to be
 * present in the script disk under the toplevel directory.
 */
void osinfo_install_config_set_pre_install_drivers_disk(OsinfoInstallConfig *config,
                                                        const gchar *disk)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_PRE_INSTALL_DRIVERS_DISK,
                            disk);
}

/**
 * osinfo_install_config_get_pre_install_drivers_disk:
 * @config: the install config
 *
 * Returns: The disk on which pre-installation drivers are located, or NULL if
 * its not set using #osinfo_install_config_set_pre_install_drivers_disk().
 */
const gchar *osinfo_install_config_get_pre_install_drivers_disk(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value
            (OSINFO_ENTITY(config),
             OSINFO_INSTALL_CONFIG_PROP_PRE_INSTALL_DRIVERS_DISK);
}

/**
 * osinfo_install_config_set_pre_install_drivers_location:
 * @config: the install config
 * @location: the location
 *
 * Specify the location on which drivers to be installed at the very beginning of
 * installation, are available. Please read documentation on
 * #osinfo_install_config_set_pre_install_drivers_disk() for more information
 * about pre-installation of drivers.
 *
 * Please read documentation on #osinfo_install_config_set_avatar_location() for
 * explanation on the format of @location string.
 */
void osinfo_install_config_set_pre_install_drivers_location(OsinfoInstallConfig *config,
                                                            const gchar *location)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_PRE_INSTALL_DRIVERS_LOCATION,
                            location);
}

/**
 * osinfo_install_config_get_pre_install_drivers_location:
 * @config: the install config
 *
 * Returns: The location on which pre-installation drivers are located, or NULL if
 * its not set using #osinfo_install_config_set_pre_install_drivers_location().
 */
const gchar *osinfo_install_config_get_pre_install_drivers_location(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value
            (OSINFO_ENTITY(config),
             OSINFO_INSTALL_CONFIG_PROP_PRE_INSTALL_DRIVERS_LOCATION);
}

/**
 * osinfo_install_config_set_post_install_drivers_disk:
 * @config: the install config
 * @disk: the target disk
 *
 * Specify the disk on which drivers to be installed at the end of installation,
 * are available.
 *
 * Please read documentation on #osinfo_install_config_set_target_disk() for
 * explanation on the format of @disk string.
 *
 * NOTE: Not every install script supports post-installation of drivers. Use
 * #osinfo_install_script_get_can_post_install_drivers() to find out if an
 * install script supports it.
 */
void osinfo_install_config_set_post_install_drivers_disk(OsinfoInstallConfig *config,
                                                         const gchar *disk)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_POST_INSTALL_DRIVERS_DISK,
                            disk);
}

/**
 * osinfo_install_config_get_post_install_drivers_disk:
 * @config: the install config
 *
 * Returns: The disk on which post-installation drivers are located, or NULL if
 * its not set using #osinfo_install_config_set_post_install_drivers_disk().
 */
const gchar *osinfo_install_config_get_post_install_drivers_disk(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value
            (OSINFO_ENTITY(config),
             OSINFO_INSTALL_CONFIG_PROP_POST_INSTALL_DRIVERS_DISK);
}

/**
 * osinfo_install_config_set_post_install_drivers_location:
 * @config: the install config
 * @location: the location of avatar
 *
 * Specify the disk on which drivers to be installed at the end of installation,
 * are available.
 *
 * Please read documentation on #osinfo_install_config_set_avatar_location() for
 * explanation on the format of @location string.
 *
 * NOTE: Not every install script supports post-installation of drivers. Use
 * #osinfo_install_script_get_can_post_install_drivers() to find out if an
 * install script supports it.
 */
void osinfo_install_config_set_post_install_drivers_location(OsinfoInstallConfig *config,
                                                             const gchar *location)
{
    osinfo_entity_set_param(OSINFO_ENTITY(config),
                            OSINFO_INSTALL_CONFIG_PROP_POST_INSTALL_DRIVERS_LOCATION,
                            location);
}

/**
 * osinfo_install_config_get_post_install_drivers_location:
 * @config: the install config
 *
 * Returns: The disk on which post-installation drivers are located, or NULL if
 * its not set using #osinfo_install_config_set_post_install_drivers_location().
 */
const gchar *osinfo_install_config_get_post_install_drivers_location(OsinfoInstallConfig *config)
{
    return osinfo_entity_get_param_value
            (OSINFO_ENTITY(config),
             OSINFO_INSTALL_CONFIG_PROP_POST_INSTALL_DRIVERS_LOCATION);
}

void osinfo_install_config_set_config_params(OsinfoInstallConfig *config,
                                             OsinfoInstallConfigParamList *config_params)
{
    if (config->priv->config_params != NULL)
        g_object_unref(config->priv->config_params);
    if (config_params != NULL)
        config->priv->config_params = g_object_ref(G_OBJECT(config_params));
    else
        config->priv->config_params = NULL;
}

OsinfoInstallConfigParamList *osinfo_install_config_get_config_params(OsinfoInstallConfig *config)
{
    return config->priv->config_params;
}


static OsinfoDatamap *
osinfo_install_config_get_param_datamap(OsinfoInstallConfig *config,
                                        const gchar *param_name)
{
    OsinfoEntity *entity;
    OsinfoInstallConfigParam *param;

    if (!config->priv->config_params)
        return NULL;

    entity = osinfo_list_find_by_id(OSINFO_LIST(config->priv->config_params),
                                    param_name);
    if (entity == NULL) {
        g_debug("%s is not a known parameter for this config", param_name);
        return NULL;
    }

    param = OSINFO_INSTALL_CONFIG_PARAM(entity);;
    return osinfo_install_config_param_get_value_map(param);
}


GList *
osinfo_install_config_get_param_value_list(OsinfoInstallConfig *config,
                                           const gchar *key)
{
    GList *values;
    GList *it;
    OsinfoDatamap *map;

    values = osinfo_entity_get_param_value_list(OSINFO_ENTITY(config), key);
    if (values == NULL)
        return NULL;

    map = osinfo_install_config_get_param_datamap(config, key);
    if (map != NULL) {
        for (it = values; it != NULL; it = it->next) {
            const char *transformed_value;
            transformed_value = osinfo_datamap_lookup(map, it->data);
            if (transformed_value == NULL) {
                continue;
            }
            it->data = (gpointer)transformed_value;
        }
    }

    return values;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
