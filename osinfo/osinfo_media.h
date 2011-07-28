/*
 * libosinfo: An installation media for a (guest) OS
 *
 * Copyright (C) 2009-2011 Red Hat, Inc
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
 *   Zeeshan Ali <zeenix@redhat.com>
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <glib-object.h>
#include <osinfo/osinfo_entity.h>

#ifndef __OSINFO_MEDIA_H__
#define __OSINFO_MEDIA_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_MEDIA                  (osinfo_media_get_type ())
#define OSINFO_MEDIA(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_MEDIA, OsinfoMedia))
#define OSINFO_IS_MEDIA(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_MEDIA))
#define OSINFO_MEDIA_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_MEDIA, OsinfoMediaClass))
#define OSINFO_IS_MEDIA_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_MEDIA))
#define OSINFO_MEDIA_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_MEDIA, OsinfoMediaClass))

typedef struct _OsinfoMedia        OsinfoMedia;

typedef struct _OsinfoMediaClass   OsinfoMediaClass;

typedef struct _OsinfoMediaPrivate OsinfoMediaPrivate;

#define OSINFO_MEDIA_PROP_ARCHITECTURE "architecture"
#define OSINFO_MEDIA_PROP_URL          "url"
#define OSINFO_MEDIA_PROP_VOLUME_ID    "volume-id"
#define OSINFO_MEDIA_PROP_SYSTEM_ID    "system-id"
#define OSINFO_MEDIA_PROP_PUBLISHER_ID "publisher-id"

/* object */
struct _OsinfoMedia
{
    OsinfoEntity parent_instance;

    /* public */

    /* private */
    OsinfoMediaPrivate *priv;
};

/* class */
struct _OsinfoMediaClass
{
    OsinfoEntityClass parent_class;

    /* class members */
};

GType osinfo_media_get_type(void);

OsinfoMedia *osinfo_media_new(const gchar *id, const gchar *architecture);

const gchar *osinfo_media_get_architecture(OsinfoMedia *media);
const gchar *osinfo_media_get_url(OsinfoMedia *media);
const gchar *osinfo_media_get_volume_id(OsinfoMedia *media);
const gchar *osinfo_media_get_system_id(OsinfoMedia *media);
const gchar *osinfo_media_get_publisher_id(OsinfoMedia *media);

#endif /* __OSINFO_MEDIA_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
