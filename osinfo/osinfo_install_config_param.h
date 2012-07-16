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
 *   Fabiano Fidêncio <fabiano@fidencio.org>
 */

#include <glib-object.h>

#ifndef __OSINFO_INSTALL_CONFIG_PARAM_H__
#define __OSINFO_INSTALL_CONFIG_PARAM_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_INSTALL_CONFIG_PARAM            (osinfo_install_config_param_get_type ())
#define OSINFO_INSTALL_CONFIG_PARAM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_INSTALL_CONFIG_PARAM, OsinfoInstallConfigParam))
#define OSINFO_IS_INSTALL_CONFIG_PARAM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_INSTALL_CONFIG_PARAM))
#define OSINFO_INSTALL_CONFIG_PARAM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_INSTALL_CONFIG_PARAM, OsinfoInstallConfigParamClass))
#define OSINFO_IS_INSTALL_CONFIG_PARAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_INSTALL_CONFIG_PARAM))
#define OSINFO_INSTALL_CONFIG_PARAM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_INSTALL_CONFIG_PARAM, OsinfoInstallConfigParamClass))

typedef struct _OsinfoInstallConfigParam        OsinfoInstallConfigParam;
typedef struct _OsinfoInstallConfigParamClass   OsinfoInstallConfigParamClass;
typedef struct _OsinfoInstallConfigParamPrivate OsinfoInstallConfigParamPrivate;

typedef enum {
    OSINFO_INSTALL_CONFIG_PARAM_POLICY_NONE,
    OSINFO_INSTALL_CONFIG_PARAM_POLICY_REQUIRED,
    OSINFO_INSTALL_CONFIG_PARAM_POLICY_OPTIONAL,
} OsinfoInstallConfigParamPolicy;

/* object */
struct _OsinfoInstallConfigParam
{
    OsinfoEntity parent_instance;
    /* public */

    /* private */
    OsinfoInstallConfigParamPrivate *priv;
};

/* class */
struct _OsinfoInstallConfigParamClass
{
    OsinfoEntityClass parent_class;

    /* class members */
};

GType osinfo_install_config_param_get_type(void);

OsinfoInstallConfigParam *osinfo_install_config_param_new(const gchar *name, const gchar *policy);

const gchar *osinfo_install_config_param_get_name(const OsinfoInstallConfigParam *config_param);

OsinfoInstallConfigParamPolicy osinfo_install_config_param_get_policy(const OsinfoInstallConfigParam *config_param);


#endif /* __OSINFO_INSTALL_CONFIG_PARAM_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
