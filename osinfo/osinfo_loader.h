/*
 * libosinfo: Database loader
 *
 * Copyright (C) 2009-2010 Red Hat, Inc
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
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

/*
 * To get a loader handle, we construct one with a construct-time only
 * backing data directory. It is already considered to be initialized
 * on return from the constructor, and ready to do work.
 *
 * To close it, we call the destructor on it.
 * Setting parameters on it will work if it's not a construct-time only
 * parameter. Reading will always work. Currently the backing directory and
 * libvirt version are the only parameters.
 *
 * The loader object contains information related to three main classes of
 * objects: hypervisors, operating systems and devices.
 */

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
    GObjectClass parent_class;

    /* class members */
};

GType osinfo_loader_get_type(void);

OsinfoLoader *osinfo_loader_new(void);

OsinfoDb *osinfo_loader_get_db(OsinfoLoader *loader);

void osinfo_loader_process_path(OsinfoLoader *loader, const gchar *path, GError **err);
void osinfo_loader_process_uri(OsinfoLoader *loader, const gchar *uri, GError **err);


#endif /* __OSINFO_LOADER_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
