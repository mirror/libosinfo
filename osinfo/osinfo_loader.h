/*
 * libosinfo: Database loader
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
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <glib-object.h>
#include <osinfo/osinfo_db.h>

#ifndef __OSINFO_LOADER_H__
#define __OSINFO_LOADER_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_LOADER                  (osinfo_loader_get_type ())
#define OSINFO_LOADER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_LOADER, OsinfoLoader))
#define OSINFO_IS_LOADER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_LOADER))
#define OSINFO_LOADER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_LOADER, OsinfoLoaderClass))
#define OSINFO_IS_LOADER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_LOADER))
#define OSINFO_LOADER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_LOADER, OsinfoLoaderClass))

typedef struct _OsinfoLoader        OsinfoLoader;

typedef struct _OsinfoLoaderClass   OsinfoLoaderClass;

typedef struct _OsinfoLoaderPrivate OsinfoLoaderPrivate;

/* object */
struct _OsinfoLoader
{
    GObject parent_instance;

    /* public */

    /* private */
    OsinfoLoaderPrivate *priv;
};

/* class */
struct _OsinfoLoaderClass
{
    /*< private >*/
    GObjectClass parent_class;

    /* class members */
};

GType osinfo_loader_get_type(void);

OsinfoLoader *osinfo_loader_new(void);

OsinfoDb *osinfo_loader_get_db(OsinfoLoader *loader);

void osinfo_loader_process_path(OsinfoLoader *loader, const gchar *path, GError **err);
void osinfo_loader_process_uri(OsinfoLoader *loader, const gchar *uri, GError **err);
void osinfo_loader_process_default_path(OsinfoLoader *loader, GError **err);
void osinfo_loader_process_system_path(OsinfoLoader *loader, GError **err);
void osinfo_loader_process_local_path(OsinfoLoader *loader, GError **err);
void osinfo_loader_process_user_path(OsinfoLoader *loader, GError **err);


#endif /* __OSINFO_LOADER_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
