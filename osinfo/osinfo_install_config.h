/*
 * libosinfo: OS installation configuration
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

#include <glib-object.h>

#ifndef __OSINFO_INSTALL_CONFIG_H__
#define __OSINFO_INSTALL_CONFIG_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_INSTALL_CONFIG                  (osinfo_install_config_get_type ())
#define OSINFO_INSTALL_CONFIG(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_INSTALL_CONFIG, OsinfoInstallConfig))
#define OSINFO_IS_INSTALL_CONFIG(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_INSTALL_CONFIG))
#define OSINFO_INSTALL_CONFIG_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_INSTALL_CONFIG, OsinfoInstallConfigClass))
#define OSINFO_IS_INSTALL_CONFIG_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_INSTALL_CONFIG))
#define OSINFO_INSTALL_CONFIG_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_INSTALL_CONFIG, OsinfoInstallConfigClass))

#define OSINFO_INSTALL_CONFIG_PROP_HARDWARE_ARCH  "hardware-arch"

#define OSINFO_INSTALL_CONFIG_PROP_L10N_TIMEZONE  "l10n-timezone"
#define OSINFO_INSTALL_CONFIG_PROP_L10N_LANGUAGE  "l10n-language"
#define OSINFO_INSTALL_CONFIG_PROP_L10N_KEYBOARD  "l10n-keyboard"

#define OSINFO_INSTALL_CONFIG_PROP_ADMIN_PASSWORD "admin-password"

#define OSINFO_INSTALL_CONFIG_PROP_USER_PASSWORD  "user-password"
#define OSINFO_INSTALL_CONFIG_PROP_USER_LOGIN     "user-login"
#define OSINFO_INSTALL_CONFIG_PROP_USER_REALNAME  "user-realname"
#define OSINFO_INSTALL_CONFIG_PROP_USER_AUTOLOGIN "user-autologin"
#define OSINFO_INSTALL_CONFIG_PROP_USER_ADMIN     "user-admin"

#define OSINFO_INSTALL_CONFIG_PROP_REG_LOGIN      "reg-login"
#define OSINFO_INSTALL_CONFIG_PROP_REG_PASSWORD   "reg-password"
#define OSINFO_INSTALL_CONFIG_PROP_REG_PRODUCTKEY "reg-product-key"

#define OSINFO_INSTALL_CONFIG_PROP_HOSTNAME    "hostname"
#define OSINFO_INSTALL_CONFIG_PROP_TARGET_DISK "target-disk"
#define OSINFO_INSTALL_CONFIG_PROP_SCRIPT_DISK "script-disk"

#define OSINFO_INSTALL_CONFIG_PROP_AVATAR_LOCATION "avatar-location"
#define OSINFO_INSTALL_CONFIG_PROP_AVATAR_DISK     "avatar-disk"

#define OSINFO_INSTALL_CONFIG_PROP_PRE_INSTALL_DRIVERS_DISK "pre-install-drivers-disk"
#define OSINFO_INSTALL_CONFIG_PROP_PRE_INSTALL_DRIVERS_LOCATION "pre-install-drivers-location"

#define OSINFO_INSTALL_CONFIG_PROP_POST_INSTALL_DRIVERS_DISK "post-install-drivers-disk"
#define OSINFO_INSTALL_CONFIG_PROP_POST_INSTALL_DRIVERS_LOCATION "post-install-drivers-location"

typedef struct _OsinfoInstallConfig        OsinfoInstallConfig;
typedef struct _OsinfoInstallConfigClass   OsinfoInstallConfigClass;
typedef struct _OsinfoInstallConfigPrivate OsinfoInstallConfigPrivate;

/* object */
struct _OsinfoInstallConfig
{
    OsinfoEntity parent_instance;

    /* public */

    /* private */
    OsinfoInstallConfigPrivate *priv;
};

/* class */
struct _OsinfoInstallConfigClass
{
    OsinfoEntityClass parent_class;

    /* class members */
};

GType osinfo_install_config_get_type(void);

OsinfoInstallConfig *osinfo_install_config_new(const gchar *id);

void osinfo_install_config_set_hardware_arch(OsinfoInstallConfig *config,
                                             const gchar *arch);
const gchar *osinfo_install_config_get_hardware_arch(OsinfoInstallConfig *config);

void osinfo_install_config_set_l10n_keyboard(OsinfoInstallConfig *config,
                                             const gchar *keyboard);
const gchar *osinfo_install_config_get_l10n_keyboard(OsinfoInstallConfig *config);

void osinfo_install_config_set_l10n_language(OsinfoInstallConfig *config,
                                             const gchar *language);
const gchar *osinfo_install_config_get_l10n_language(OsinfoInstallConfig *config);

void osinfo_install_config_set_l10n_timezone(OsinfoInstallConfig *config,
                                             const gchar *timezone);
const gchar *osinfo_install_config_get_l10n_timezone(OsinfoInstallConfig *config);




void osinfo_install_config_set_admin_password(OsinfoInstallConfig *config,
                                              const gchar *password);
const gchar *osinfo_install_config_get_admin_password(OsinfoInstallConfig *config);




void osinfo_install_config_set_user_login(OsinfoInstallConfig *config,
                                          const gchar *username);
const gchar *osinfo_install_config_get_user_login(OsinfoInstallConfig *config);


void osinfo_install_config_set_user_password(OsinfoInstallConfig *config,
                                             const gchar *password);
const gchar *osinfo_install_config_get_user_password(OsinfoInstallConfig *config);


void osinfo_install_config_set_user_realname(OsinfoInstallConfig *config,
                                             const gchar *name);
const gchar *osinfo_install_config_get_user_realname(OsinfoInstallConfig *config);


void osinfo_install_config_set_user_autologin(OsinfoInstallConfig *config,
                                              gboolean autologin);
gboolean osinfo_install_config_get_user_autologin(OsinfoInstallConfig *config);


void osinfo_install_config_set_user_administrator(OsinfoInstallConfig *config,
                                                  gboolean admin);
gboolean osinfo_install_config_get_user_administrator(OsinfoInstallConfig *config);



void osinfo_install_config_set_reg_login(OsinfoInstallConfig *config,
                                         const gchar *name);
const gchar *osinfo_install_config_get_reg_login(OsinfoInstallConfig *config);

void osinfo_install_config_set_reg_password(OsinfoInstallConfig *config,
                                            const gchar *password);
const gchar *osinfo_install_config_get_reg_password(OsinfoInstallConfig *config);

void osinfo_install_config_set_reg_product_key(OsinfoInstallConfig *config,
                                               const gchar *key);
const gchar *osinfo_install_config_get_reg_product_key(OsinfoInstallConfig *config);

void osinfo_install_config_set_hostname(OsinfoInstallConfig *config,
                                        const gchar *hostname);
const gchar *osinfo_install_config_get_hostname(OsinfoInstallConfig *config);

void osinfo_install_config_set_target_disk(OsinfoInstallConfig *config,
                                           const gchar *disk);
const gchar *osinfo_install_config_get_target_disk(OsinfoInstallConfig *config);

void osinfo_install_config_set_script_disk(OsinfoInstallConfig *config,
                                           const gchar *disk);
const gchar *osinfo_install_config_get_script_disk(OsinfoInstallConfig *config);

void osinfo_install_config_set_avatar_location(OsinfoInstallConfig *config,
                                               const gchar *location);
const gchar *osinfo_install_config_get_avatar_location(OsinfoInstallConfig *config);

void osinfo_install_config_set_avatar_disk(OsinfoInstallConfig *config,
                                           const gchar *disk);
const gchar *osinfo_install_config_get_avatar_disk(OsinfoInstallConfig *config);

void osinfo_install_config_set_pre_install_drivers_disk(OsinfoInstallConfig *config,
                                                        const gchar *disk);
const gchar *osinfo_install_config_get_pre_install_drivers_disk(OsinfoInstallConfig *config);
void osinfo_install_config_set_pre_install_drivers_location(OsinfoInstallConfig *config,
                                                            const gchar *location);
const gchar *osinfo_install_config_get_pre_install_drivers_location(OsinfoInstallConfig *config);

void osinfo_install_config_set_post_install_drivers_disk(OsinfoInstallConfig *config,
                                                         const gchar *disk);
const gchar *osinfo_install_config_get_post_install_drivers_disk(OsinfoInstallConfig *config);
void osinfo_install_config_set_post_install_drivers_location(OsinfoInstallConfig *config,
                                                             const gchar *location);
const gchar *osinfo_install_config_get_post_install_drivers_location(OsinfoInstallConfig *config);

#endif /* __OSINFO_INSTALL_CONFIG_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
