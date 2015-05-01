/*
 * libosinfo: OS installation script
 *
 * Copyright (C) 2009-2012, 2014 Red Hat, Inc.
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
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <glib-object.h>
#include <gio/gio.h>
#include <osinfo/osinfo_install_config_param.h>
#include <osinfo/osinfo_avatar_format.h>

#ifndef __OSINFO_INSTALL_SCRIPT_H__
#define __OSINFO_INSTALL_SCRIPT_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_INSTALL_SCRIPT                  (osinfo_install_script_get_type ())
#define OSINFO_INSTALL_SCRIPT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_INSTALL_SCRIPT, OsinfoInstallScript))
#define OSINFO_IS_INSTALL_SCRIPT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_INSTALL_SCRIPT))
#define OSINFO_INSTALL_SCRIPT_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_INSTALL_SCRIPT, OsinfoInstallScriptClass))
#define OSINFO_IS_INSTALL_SCRIPT_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_INSTALL_SCRIPT))
#define OSINFO_INSTALL_SCRIPT_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_INSTALL_SCRIPT, OsinfoInstallScriptClass))

typedef struct _OsinfoOs        OsinfoOs;
typedef struct _OsinfoMedia     OsinfoMedia;
typedef struct _OsinfoInstallScript        OsinfoInstallScript;
typedef struct _OsinfoInstallScriptClass   OsinfoInstallScriptClass;
typedef struct _OsinfoInstallScriptPrivate OsinfoInstallScriptPrivate;

#define OSINFO_INSTALL_SCRIPT_PROFILE_JEOS    "jeos"
#define OSINFO_INSTALL_SCRIPT_PROFILE_DESKTOP "desktop"

#define OSINFO_INSTALL_SCRIPT_PROP_TEMPLATE_URI       "template-uri"
#define OSINFO_INSTALL_SCRIPT_PROP_TEMPLATE_DATA      "template-data"
#define OSINFO_INSTALL_SCRIPT_PROP_PROFILE            "profile"
#define OSINFO_INSTALL_SCRIPT_PROP_PRODUCT_KEY_FORMAT "product-key-format"
#define OSINFO_INSTALL_SCRIPT_PROP_EXPECTED_FILENAME  "expected-filename"
#define OSINFO_INSTALL_SCRIPT_PROP_PATH_FORMAT        "path-format"
#define OSINFO_INSTALL_SCRIPT_PROP_CAN_PRE_INSTALL_DRIVERS "can-pre-install-drivers"
#define OSINFO_INSTALL_SCRIPT_PROP_CAN_POST_INSTALL_DRIVERS "can-post-install-drivers"
#define OSINFO_INSTALL_SCRIPT_PROP_NEEDS_INTERNET     "needs-internet"
#define OSINFO_INSTALL_SCRIPT_PROP_PRE_INSTALL_DRIVERS_SIGNING_REQ "pre-install-drivers-signing-req"
#define OSINFO_INSTALL_SCRIPT_PROP_POST_INSTALL_DRIVERS_SIGNING_REQ "post-install-drivers-signing-req"
#define OSINFO_INSTALL_SCRIPT_PROP_INJECTION_METHOD "injection-method"

/* object */
struct _OsinfoInstallScript
{
    OsinfoEntity parent_instance;

    /* public */

    /* private */
    OsinfoInstallScriptPrivate *priv;
};

/* class */
struct _OsinfoInstallScriptClass
{
    /*< private >*/
    OsinfoEntityClass parent_class;

    /* class members */
};

/**
 * OsinfoPathFormat:
 * @OSINFO_PATH_FORMAT_UNIX: Unix/Linux path format, e.g /home/foo/bar.txt
 * @OSINFO_PATH_FORMAT_DOS: DOS/Windows path format, e.g \My Documents\bar.txt
 */
typedef enum {
    OSINFO_PATH_FORMAT_UNIX,
    OSINFO_PATH_FORMAT_DOS
} OsinfoPathFormat;

/**
 * OsinfoDeviceDriverSigningReq:
 *
 * @OSINFO_DEVICE_DRIVER_SIGNING_REQ_NONE: Script do not require device drivers
 * to be signed.
 *
 * @OSINFO_DEVICE_DRIVER_SIGNING_REQ_STRICT: Script must only be given signed
 * device drivers. Some scripts will allow overriding this requirement through
 * #osinfo_install_config_set_driver_signing function. You can query if a
 * script supports this by checking if
 * #OSINFO_INSTALL_CONFIG_PROP_DRIVER_SIGNING configuration parameter is used
 * by the script in question (or other scripts in the same profile).
 *
 * @OSINFO_DEVICE_DRIVER_SIGNING_REQ_WARN: A warning will be issued by OS
 * installer if device drivers are not signed and most probably require user
 * input (and thus breaking unattended installation). See
 * #OSINFO_DEVICE_DRIVER_SIGNING_REQ_STRICT on how this warning can be disabled
 * for some scripts.
 */
typedef enum {
    OSINFO_DEVICE_DRIVER_SIGNING_REQ_NONE,
    OSINFO_DEVICE_DRIVER_SIGNING_REQ_STRICT,
    OSINFO_DEVICE_DRIVER_SIGNING_REQ_WARN
} OsinfoDeviceDriverSigningReq;

/**
 * OsinfoInstallScriptInjectionMethod:
 *
 * @OSINFO_INSTALL_SCRIPT_INJECTION_METHOD_CDROM: Support injection of the
 * installation script trough a CD-ROM.
 * @OSINFO_INSTALL_SCRIPT_INJECTION_METHOD_DISK: Support injection of the
 * installation script trough a disk.
 * @OSINFO_INSTALL_SCRIPT_INJECTION_METHOD_FLOPPY: Support injection of the
 * installation script trough a floppy disk.
 * @OSINFO_INSTALL_SCRIPT_INJECTION_METHOD_INITRD: Support injection of the
 * installation script trough the initrd.
 * @OSINFO_INSTALL_SCRIPT_INJECTION_METHOD_WEB: Support injection of the
 * installation script from the web.
 */
typedef enum {
    OSINFO_INSTALL_SCRIPT_INJECTION_METHOD_CDROM  = 1 << 0,
    OSINFO_INSTALL_SCRIPT_INJECTION_METHOD_DISK   = 1 << 1,
    OSINFO_INSTALL_SCRIPT_INJECTION_METHOD_FLOPPY = 1 << 2,
    OSINFO_INSTALL_SCRIPT_INJECTION_METHOD_INITRD = 1 << 3,
    OSINFO_INSTALL_SCRIPT_INJECTION_METHOD_WEB    = 1 << 4,
} OsinfoInstallScriptInjectionMethod;

GType osinfo_install_script_get_type(void);

OsinfoInstallScript *osinfo_install_script_new(const gchar *id);
OsinfoInstallScript *osinfo_install_script_new_uri(const gchar *id,
                                                   const gchar *profile,
                                                   const gchar *templateUri);
OsinfoInstallScript *osinfo_install_script_new_data(const gchar *id,
                                                    const gchar *profile,
                                                    const gchar *templateData);

const gchar *osinfo_install_script_get_template_uri(OsinfoInstallScript *script);
const gchar *osinfo_install_script_get_template_data(OsinfoInstallScript *script);
const gchar *osinfo_install_script_get_profile(OsinfoInstallScript *script);

const gchar *osinfo_install_script_get_product_key_format(OsinfoInstallScript *script);

void osinfo_install_script_set_output_prefix(OsinfoInstallScript *script, const gchar *prefix);

const gchar *osinfo_install_script_get_output_prefix(OsinfoInstallScript *script);

const gchar *osinfo_install_script_get_output_filename(OsinfoInstallScript *script);

const gchar *osinfo_install_script_get_expected_filename(OsinfoInstallScript *script);

OsinfoAvatarFormat *osinfo_install_script_get_avatar_format(OsinfoInstallScript *script);

void osinfo_install_script_generate_async(OsinfoInstallScript *script,
                                          OsinfoOs *os,
                                          OsinfoInstallConfig *config,
                                          GCancellable *cancellable,
                                          GAsyncReadyCallback callback,
                                          gpointer user_data);

gchar *osinfo_install_script_generate_finish(OsinfoInstallScript *script,
                                             GAsyncResult *res,
                                             GError **error);

gchar *osinfo_install_script_generate(OsinfoInstallScript *script,
                                      OsinfoOs *os,
                                      OsinfoInstallConfig *config,
                                      GCancellable *cancellable,
                                      GError **error);

void osinfo_install_script_generate_output_async(OsinfoInstallScript *script,
                                                 OsinfoOs *os,
                                                 OsinfoInstallConfig *config,
                                                 GFile *output_dir,
                                                 GCancellable *cancellable,
                                                 GAsyncReadyCallback callback,
                                                 gpointer user_data);

GFile *osinfo_install_script_generate_output_finish(OsinfoInstallScript *script,
                                                    GAsyncResult *res,
                                                    GError **error);

GFile *osinfo_install_script_generate_output(OsinfoInstallScript *script,
                                             OsinfoOs *os,
                                             OsinfoInstallConfig *config,
                                             GFile *output_dir,
                                             GCancellable *cancellable,
                                             GError **error);

void osinfo_install_script_generate_for_media_async(OsinfoInstallScript *script,
                                                    OsinfoMedia *media,
                                                    OsinfoInstallConfig *config,
                                                    GCancellable *cancellable,
                                                    GAsyncReadyCallback callback,
                                                    gpointer user_data);
gchar *osinfo_install_script_generate_for_media_finish(OsinfoInstallScript *script,
                                                       GAsyncResult *res,
                                                       GError **error);
gchar *osinfo_install_script_generate_for_media(OsinfoInstallScript *script,
                                                OsinfoMedia *media,
                                                OsinfoInstallConfig *config,
                                                GCancellable *cancellable,
                                                GError **error);

gchar *osinfo_install_script_generate_command_line(OsinfoInstallScript *script,
                                                   OsinfoOs *os,
                                                   OsinfoInstallConfig *config);

gboolean osinfo_install_script_has_config_param(const OsinfoInstallScript *script, const OsinfoInstallConfigParam *config_param);

gboolean osinfo_install_script_has_config_param_name(const OsinfoInstallScript *script, const gchar *name);

OsinfoInstallConfigParam *osinfo_install_script_get_config_param(const OsinfoInstallScript *script, const gchar *name);

GList *osinfo_install_script_get_config_param_list(const OsinfoInstallScript *script);
OsinfoInstallConfigParamList *osinfo_install_script_get_config_params(const OsinfoInstallScript *script);
OsinfoPathFormat osinfo_install_script_get_path_format(OsinfoInstallScript *script);

gboolean osinfo_install_script_get_can_pre_install_drivers(OsinfoInstallScript *script);
gboolean osinfo_install_script_get_can_post_install_drivers(OsinfoInstallScript *script);

int osinfo_install_script_get_pre_install_drivers_signing_req(OsinfoInstallScript *script);
int osinfo_install_script_get_post_install_drivers_signing_req(OsinfoInstallScript *script);

unsigned int osinfo_install_script_get_injection_methods(OsinfoInstallScript *script);

gboolean osinfo_install_script_get_needs_internet(OsinfoInstallScript *script);

#endif /* __OSINFO_INSTALL_SCRIPT_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
