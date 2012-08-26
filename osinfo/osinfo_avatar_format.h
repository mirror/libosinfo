/*
 * libosinfo: OS installation avatar information
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *   Fabiano Fidêncio <fabiano@fidencio.org>
 *   Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
 */

#include <glib-object.h>

#ifndef __OSINFO_AVATAR_FORMAT_H__
#define __OSINFO_AVATAR_FORMAT_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_AVATAR_FORMAT              \
        (osinfo_avatar_format_get_type ())

#define OSINFO_AVATAR_FORMAT(obj)              \
        (G_TYPE_CHECK_INSTANCE_CAST ((obj),    \
         OSINFO_TYPE_AVATAR_FORMAT,            \
         OsinfoAvatarFormat))

#define OSINFO_IS_AVATAR_FORMAT(obj)           \
        (G_TYPE_CHECK_INSTANCE_TYPE ((obj),    \
         OSINFO_TYPE_AVATAR_FORMAT))

#define OSINFO_AVATAR_FORMAT_CLASS(klass)      \
        (G_TYPE_CHECK_CLASS_CAST ((klass),     \
         OSINFO_TYPE_AVATAR_FORMAT,            \
         OsinfoAvatarFormatClass))

#define OSINFO_IS_AVATAR_FORMAT_CLASS(klass)   \
        (G_TYPE_CHECK_CLASS_TYPE ((klass),     \
         OSINFO_TYPE_AVATAR_FORMAT))

#define OSINFO_AVATAR_FORMAT_GET_CLASS(obj)    \
        (G_TYPE_INSTANCE_GET_CLASS ((obj),     \
         OSINFO_TYPE_AVATAR_FORMAT,            \
         OsinfoAvatarFormatClass))

typedef struct _OsinfoAvatarFormat OsinfoAvatarFormat;
typedef struct _OsinfoAvatarFormatClass OsinfoAvatarFormatClass;

#define OSINFO_AVATAR_FORMAT_PROP_MIME_TYPE "mime-type"
#define OSINFO_AVATAR_FORMAT_PROP_WIDTH     "width"
#define OSINFO_AVATAR_FORMAT_PROP_HEIGHT    "height"
#define OSINFO_AVATAR_FORMAT_PROP_ALPHA     "alpha"

/* object */
struct _OsinfoAvatarFormat
{
    OsinfoEntity parent_instance;
};

/* class */
struct _OsinfoAvatarFormatClass
{
    OsinfoEntityClass parent_class;
};

GType osinfo_avatar_format_get_type(void);

OsinfoAvatarFormat *osinfo_avatar_format_new(void);

GList *osinfo_avatar_format_get_mime_types(OsinfoAvatarFormat *avatar);
gint osinfo_avatar_format_get_width(OsinfoAvatarFormat *avatar);
gint osinfo_avatar_format_get_height(OsinfoAvatarFormat *avatar);
gboolean osinfo_avatar_format_get_alpha(OsinfoAvatarFormat *avatar);

#endif /* __OSINFO_AVATAR_FORMAT_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
