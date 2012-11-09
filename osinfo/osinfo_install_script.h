/*
 * libosinfo: OS installation script
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
#include <gio/gio.h>
#include <osinfo/osinfo_install_config_param.h>

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
    OsinfoEntityClass parent_class;

    /* class members */
};

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

gboolean osinfo_install_script_has_config_param(const OsinfoInstallScript *script, const OsinfoInstallConfigParam *config_param);

gboolean osinfo_install_script_has_config_param_name(const OsinfoInstallScript *script, const gchar *name);

OsinfoInstallConfigParam *osinfo_install_script_get_config_param(const OsinfoInstallScript *script, const gchar *name);

void osinfo_install_script_add_config_param(OsinfoInstallScript *script, OsinfoInstallConfigParam *param);

GList *osinfo_install_script_get_config_param_list(const OsinfoInstallScript *script);

#endif /* __OSINFO_INSTALL_SCRIPT_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
