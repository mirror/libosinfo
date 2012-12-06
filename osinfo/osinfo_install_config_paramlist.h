/*
 * libosinfo: a list of installation configuration parameters
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
 *   Arjun Roy <arroy@redhat.com>
 *   Christophe Fergeau <cfergeau@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 *   Zeeshan Ali <zeenix@redhat.com>
 */

#include <glib-object.h>
#include <osinfo/osinfo_list.h>

#ifndef __OSINFO_INSTALL_CONFIG_PARAMLIST_H__
#define __OSINFO_INSTALL_CONFIG_PARAMLIST_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_INSTALL_CONFIG_PARAMLIST                  (osinfo_install_config_paramlist_get_type ())
#define OSINFO_INSTALL_CONFIG_PARAMLIST(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_INSTALL_CONFIG_PARAMLIST, OsinfoInstallConfigParamList))
#define OSINFO_IS_INSTALL_CONFIG_PARAMLIST(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_INSTALL_CONFIG_PARAMLIST))
#define OSINFO_INSTALL_CONFIG_PARAMLIST_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_INSTALL_CONFIG_PARAMLIST, OsinfoInstallConfigParamListClass))
#define OSINFO_IS_INSTALL_CONFIG_PARAMLIST_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_INSTALL_CONFIG_PARAMLIST))
#define OSINFO_INSTALL_CONFIG_PARAMLIST_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_INSTALL_CONFIG_PARAMLIST, OsinfoInstallConfigParamListClass))

typedef struct _OsinfoInstallConfigParamList        OsinfoInstallConfigParamList;

typedef struct _OsinfoInstallConfigParamListClass   OsinfoInstallConfigParamListClass;

typedef struct _OsinfoInstallConfigParamListPrivate OsinfoInstallConfigParamListPrivate;

/* object */
struct _OsinfoInstallConfigParamList
{
    OsinfoList parent_instance;

    /* public */

    /* private */
    OsinfoInstallConfigParamListPrivate *priv;
};

/* class */
struct _OsinfoInstallConfigParamListClass
{
    OsinfoListClass parent_class;

    /* class members */
};

GType osinfo_install_config_paramlist_get_type(void);

OsinfoInstallConfigParamList *osinfo_install_config_paramlist_new(void);

#endif /* __OSINFO_INSTALL_CONFIG_PARAMLIST_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
