/*
 * libosinfo:
 *
 * Copyright (C) 2009-2012, 2014 Red Hat, Inc.
 * Copyright (C) 2012 Fabiano Fidêncio
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

#include <config.h>

#include <osinfo/osinfo.h>
#include <glib/gi18n-lib.h>

G_DEFINE_TYPE(OsinfoAvatarFormat, osinfo_avatar_format, OSINFO_TYPE_ENTITY);

#define OSINFO_AVATAR_FORMAT_GET_PRIVATE(obj)  \
        (G_TYPE_INSTANCE_GET_PRIVATE((obj),                \
         OSINFO_TYPE_AVATAR_FORMAT,            \
         OsinfoAvatarFormatPrivate))

/**
 * SECTION: osinfo_avatar_format
 * @short_description: The required format of avatar for an install script
 * @see_also: #OsinfoInstallScript
 */

enum {
    PROP_0,

    PROP_MIME_TYPES,
    PROP_WIDTH,
    PROP_HEIGHT,
    PROP_ALPHA,
};

static void
osinfo_avatar_format_get_property(GObject *object,
                                  guint property_id,
                                  GValue *value,
                                  GParamSpec *pspec)
{
    OsinfoAvatarFormat *avatar = OSINFO_AVATAR_FORMAT(object);

    switch (property_id)
        {
        case PROP_MIME_TYPES:
        {
            GList *mime_types;

            mime_types = osinfo_avatar_format_get_mime_types(avatar);
            g_value_set_pointer(value, mime_types);
            break;
        }
        case PROP_WIDTH:
            g_value_set_int(value,
                            osinfo_avatar_format_get_width(avatar));
            break;
        case PROP_HEIGHT:
            g_value_set_int(value,
                            osinfo_avatar_format_get_height(avatar));
            break;
        case PROP_ALPHA:
            g_value_set_boolean(value,
                                osinfo_avatar_format_get_alpha(avatar));
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
        }
}

/* Init functions */
static void
osinfo_avatar_format_class_init(OsinfoAvatarFormatClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS(klass);
    GParamSpec *pspec;

    g_klass->get_property = osinfo_avatar_format_get_property;

    /**
     * OsinfoAvatarFormat:mime-types:
     *
     * The allowed mime-types for the avatar.
     *
     * Type: GLib.List(utf8)
     * Transfer: container
     **/
    pspec = g_param_spec_pointer("mime-types",
                                "MIME Types",
                                _("The allowed mime-types for the avatar"),
                                G_PARAM_READABLE |
                                G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(g_klass,
                                    PROP_MIME_TYPES,
                                    pspec);

    /**
     * OsinfoAvatarFormat:width:
     *
     * The required width (in pixels) of the avatar.
     **/
    pspec = g_param_spec_int("width",
                             "Width",
                             _("The required width (in pixels) of the avatar"),
                             -1,
                             G_MAXINT,
                             -1,
                             G_PARAM_READABLE |
                             G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(g_klass,
                                    PROP_WIDTH,
                                    pspec);

    /**
     * OsinfoAvatarFormat:height:
     *
     * The required height (in pixels) of the avatar.
     **/
    pspec = g_param_spec_int("height",
                             "Height",
                             _("The required height (in pixels) of the avatar."),
                             -1,
                             G_MAXINT,
                             -1,
                             G_PARAM_READABLE |
                             G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(g_klass,
                                    PROP_HEIGHT,
                                    pspec);

    /**
     * OsinfoAvatarFormat:alpha:
     *
     * Whether alpha channel is supported in the avatar.
     **/
    pspec = g_param_spec_boolean("alpha",
                                 "Alpha",
                                 _("Whether alpha channel is supported in the avatar."),
                                 TRUE,
                                 G_PARAM_READABLE |
                                 G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(g_klass,
                                    PROP_ALPHA,
                                    pspec);
}

static void
osinfo_avatar_format_init(OsinfoAvatarFormat *avatar)
{
}

/**
 * osinfo_avatar_format_new:
 *
 * Construct a new user avatar file for an #OsinfoInstallScript.
 *
 * Returns: (transfer full): the necessary information to create an avatar for
 *                           an user
 */
OsinfoAvatarFormat *
osinfo_avatar_format_new(void)
{
    return g_object_new(OSINFO_TYPE_AVATAR_FORMAT, NULL);
}

/**
 * osinfo_avatar_format_get_mime_types:
 * @avatar: the avatar info
 *
 * Returns: (transfer container) (element-type utf8): the required mime-types of the avatar.
 */
GList *
osinfo_avatar_format_get_mime_types(OsinfoAvatarFormat *avatar)
{
    return osinfo_entity_get_param_value_list(OSINFO_ENTITY(avatar),
                                              OSINFO_AVATAR_FORMAT_PROP_MIME_TYPE);
}

/**
 * osinfo_avatar_format_get_width:
 * @avatar: the avatar info
 *
 * Returns: the required width (in pixels) of the avatar, or -1.
 */
gint
osinfo_avatar_format_get_width(OsinfoAvatarFormat *avatar)
{
    return osinfo_entity_get_param_value_int64(OSINFO_ENTITY(avatar),
                                               OSINFO_AVATAR_FORMAT_PROP_WIDTH);
}

/**
 * osinfo_avatar_format_get_height:
 * @avatar: the avatar info
 *
 * Returns: the required height (in pixels) of the avatar, or -1.
 */
gint
osinfo_avatar_format_get_height(OsinfoAvatarFormat *avatar)
{
    return osinfo_entity_get_param_value_int64(OSINFO_ENTITY(avatar),
                                               OSINFO_AVATAR_FORMAT_PROP_HEIGHT);
}

/**
 * osinfo_avatar_format_get_alpha:
 * @avatar: the avatar info
 *
 * Returns: Whether alpha channel is supported in the avatar.
 */
gboolean
osinfo_avatar_format_get_alpha(OsinfoAvatarFormat *avatar)
{
    return osinfo_entity_get_param_value_boolean_with_default
                (OSINFO_ENTITY(avatar), OSINFO_AVATAR_FORMAT_PROP_ALPHA, TRUE);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
