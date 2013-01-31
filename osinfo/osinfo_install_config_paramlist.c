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
 */

#include <config.h>

#include <osinfo/osinfo.h>
#include <glib/gi18n-lib.h>

G_DEFINE_TYPE (OsinfoInstallConfigParamList, osinfo_install_config_paramlist, OSINFO_TYPE_LIST);

#define OSINFO_INSTALL_CONFIG_PARAMLIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_INSTALL_CONFIG_PARAMLIST, OsinfoInstallConfigParamListPrivate))

/**
 * SECTION:osinfo_install_config_paramlist
 * @short_description: A list of installation install_config_param
 * @see_also: #OsinfoList, #OsinfoInstallConfigParam
 *
 * #OsinfoInstallConfigParamList is a list specialization that stores
 * only #OsinfoInstallConfigParam objects.
 */

struct _OsinfoInstallConfigParamListPrivate
{
    gboolean unused;
};

static void
osinfo_install_config_paramlist_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_install_config_paramlist_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_install_config_paramlist_class_init (OsinfoInstallConfigParamListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_install_config_paramlist_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoInstallConfigParamListPrivate));
}

static void
osinfo_install_config_paramlist_init (OsinfoInstallConfigParamList *list)
{
    list->priv = OSINFO_INSTALL_CONFIG_PARAMLIST_GET_PRIVATE(list);
}

/**
 * osinfo_install_config_paramlist_new:
 *
 * Construct a new install_config_param list that is initially empty.
 *
 * Returns: (transfer full): an empty install_config_param list
 */
OsinfoInstallConfigParamList *osinfo_install_config_paramlist_new(void)
{
    return g_object_new(OSINFO_TYPE_INSTALL_CONFIG_PARAMLIST,
                        "element-type", OSINFO_TYPE_INSTALL_CONFIG_PARAM,
                        NULL);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
