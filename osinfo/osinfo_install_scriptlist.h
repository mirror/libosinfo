/*
 * libosinfo: a list of installation install_script
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
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Zeeshan Ali <zeenix@redhat.com>
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <glib-object.h>
#include <osinfo/osinfo_list.h>

#ifndef __OSINFO_INSTALL_SCRIPTLIST_H__
#define __OSINFO_INSTALL_SCRIPTLIST_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_INSTALL_SCRIPTLIST                  (osinfo_install_scriptlist_get_type ())
#define OSINFO_INSTALL_SCRIPTLIST(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_INSTALL_SCRIPTLIST, OsinfoInstallScriptList))
#define OSINFO_IS_INSTALL_SCRIPTLIST(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_INSTALL_SCRIPTLIST))
#define OSINFO_INSTALL_SCRIPTLIST_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_INSTALL_SCRIPTLIST, OsinfoInstallScriptListClass))
#define OSINFO_IS_INSTALL_SCRIPTLIST_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_INSTALL_SCRIPTLIST))
#define OSINFO_INSTALL_SCRIPTLIST_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_INSTALL_SCRIPTLIST, OsinfoInstallScriptListClass))

typedef struct _OsinfoInstallScriptList        OsinfoInstallScriptList;

typedef struct _OsinfoInstallScriptListClass   OsinfoInstallScriptListClass;

typedef struct _OsinfoInstallScriptListPrivate OsinfoInstallScriptListPrivate;

/* object */
struct _OsinfoInstallScriptList
{
    OsinfoList parent_instance;

    /* public */

    /* private */
    OsinfoInstallScriptListPrivate *priv;
};

/* class */
struct _OsinfoInstallScriptListClass
{
    /*< private >*/
    OsinfoListClass parent_class;

    /* class members */
};

GType osinfo_install_scriptlist_get_type(void);

OsinfoInstallScriptList *osinfo_install_scriptlist_new(void);
G_DEPRECATED_FOR(osinfo_list_new_copy)
OsinfoInstallScriptList *osinfo_install_scriptlist_new_copy(OsinfoInstallScriptList *source);
G_DEPRECATED_FOR(osinfo_list_new_filtered)
OsinfoInstallScriptList *osinfo_install_scriptlist_new_filtered(OsinfoInstallScriptList *source, OsinfoFilter *filter);
G_DEPRECATED_FOR(osinfo_list_new_intersection)
OsinfoInstallScriptList *osinfo_install_scriptlist_new_intersection(OsinfoInstallScriptList *sourceOne, OsinfoInstallScriptList *sourceTwo);
G_DEPRECATED_FOR(osinfo_list_new_union)
OsinfoInstallScriptList *osinfo_install_scriptlist_new_union(OsinfoInstallScriptList *sourceOne, OsinfoInstallScriptList *sourceTwo);

#endif /* __OSINFO_INSTALL_SCRIPTLIST_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
