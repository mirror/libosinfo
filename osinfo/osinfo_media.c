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

#include <osinfo/osinfo.h>
#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VOLUME 32
#define MAX_SYSTEM 32
#define MAX_PUBLISHER 128

#define PVD_OFFSET 0x00008000
#define BOOTABLE_TAG "EL TORITO SPECIFICATION"

typedef struct _PrimaryVolumeDescriptor PrimaryVolumeDescriptor;

struct _PrimaryVolumeDescriptor {
    guint8 ignored[8];
    gchar  system[MAX_SYSTEM];       /* System ID */
    gchar  volume[MAX_VOLUME];       /* Volume ID */
    guint8 ignored2[246];
    gchar  publisher[MAX_PUBLISHER]; /* Publisher ID */
    guint8 ignored3[1602];
};

typedef struct _SupplementaryVolumeDescriptor SupplementaryVolumeDescriptor;

struct _SupplementaryVolumeDescriptor {
    guint8 ignored[7];
    gchar  system[MAX_SYSTEM]; /* System ID */
};

GQuark
osinfo_media_error_quark (void)
{
    static GQuark quark = 0;

    if (!quark)
        quark = g_quark_from_static_string ("osinfo-media-error");

    return quark;
}

G_DEFINE_TYPE (OsinfoMedia, osinfo_media, OSINFO_TYPE_ENTITY);

#define OSINFO_MEDIA_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                      OSINFO_TYPE_MEDIA, \
                                      OsinfoMediaPrivate))

/**
 * SECTION:osinfo_media
 * @short_description: An installation media for a (guest) OS
 * @see_also: #OsinfoOs
 *
 * #OsinfoMedia is an entity representing an installation media
 * a (guest) operating system.
 */

struct _OsinfoMediaPrivate
{
    gboolean unused;
};


static void
osinfo_media_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_media_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_media_class_init (OsinfoMediaClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_media_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoMediaPrivate));
}

static void
osinfo_media_init (OsinfoMedia *media)
{
    OsinfoMediaPrivate *priv;
    media->priv = priv = OSINFO_MEDIA_GET_PRIVATE(media);
}

OsinfoMedia *osinfo_media_new(const gchar *id,
                              const gchar *architecture)
{
    OsinfoMedia *media;
  
    media = g_object_new(OSINFO_TYPE_MEDIA,
                         "id", id,
                         NULL);

    osinfo_entity_set_param(OSINFO_ENTITY(media),
                            OSINFO_MEDIA_PROP_ARCHITECTURE,
                            architecture);

    return media;
}

/**
 * osinfo_media_create_from_location:
 * @location: the location of an installation media
 * @cancellable (allow-none): a #GCancellable, or %NULL
 * @error: The location where to store any error, or %NULL
 *
 * Creates a new #OsinfoMedia for installation media at @location. The @location
 * could be any URI that GIO can handle or a local path.
 *
 * NOTE: Currently this only works for ISO images/devices.
 *
 * Returns: (transfer full): a new #OsinfoMedia , or NULL on error
 */
OsinfoMedia *osinfo_media_create_from_location(const gchar *location,
                                               GCancellable *cancellable,
                                               GError **error)
{
    OsinfoMedia *ret = NULL;
    PrimaryVolumeDescriptor pvd;
    SupplementaryVolumeDescriptor svd;
    GFile *file;
    GFileInputStream *stream;
    gchar *uri;

    g_return_val_if_fail(location != NULL, NULL);
    g_return_val_if_fail(error == NULL || *error == NULL, NULL);

    file = g_file_new_for_commandline_arg(location);
    stream = g_file_read(file, cancellable, error);
    if (error != NULL && *error != NULL) {
        g_prefix_error(error, "Failed to open file");

        goto EXIT;
    }

    memset(&pvd, 0, sizeof(pvd));
    if (g_input_stream_skip(G_INPUT_STREAM(stream),
                            PVD_OFFSET,
                            cancellable,
                            error) < sizeof(pvd)) {
        if (*error)
            g_prefix_error(error, "Failed to skip %d bytes", PVD_OFFSET);
        else
            g_set_error(error,
                         OSINFO_MEDIA_ERROR,
                         OSINFO_MEDIA_ERROR_NO_DESCRIPTORS,
                         "No volume descriptors");

        goto EXIT;
    }

    if (g_input_stream_read(G_INPUT_STREAM(stream),
                            &pvd,
                            sizeof(pvd),
                            cancellable,
                            error) < sizeof(pvd)) {
        if (*error)
            g_prefix_error(error, "Failed to read primary volume descriptor");
        else
            g_set_error(error,
                        OSINFO_MEDIA_ERROR,
                        OSINFO_MEDIA_ERROR_NO_PVD,
                        "Primary volume descriptor unavailable");

        goto EXIT;
    }

    pvd.volume[MAX_VOLUME - 1] = 0;
    pvd.system[MAX_SYSTEM - 1] = 0;
    pvd.publisher[MAX_PUBLISHER - 1] = 0;

    if (pvd.volume[0] && (pvd.system[0] == 0 && pvd.publisher[0] == 0)) {
        g_set_error(error,
                    OSINFO_MEDIA_ERROR,
                    OSINFO_MEDIA_ERROR_INSUFFIENT_METADATA,
                    "Insufficient metadata on installation media");

        goto EXIT;
    }

    memset(&svd, 0, sizeof(svd));
    if (g_input_stream_read(G_INPUT_STREAM(stream),
                            &svd,
                            sizeof(svd),
                            cancellable,
                            error) < sizeof(svd)) {
        if (*error)
            g_prefix_error(error,
                           "Failed to read supplementary volume descriptor");
        else
            g_set_error(error,
                        OSINFO_MEDIA_ERROR,
                        OSINFO_MEDIA_ERROR_NO_SVD,
                        "Supplementary volume descriptor unavailable");

        goto EXIT;
    }

    svd.system[MAX_SYSTEM - 1] = 0;

    if (strncmp(BOOTABLE_TAG, svd.system, sizeof(BOOTABLE_TAG) != 0)) {
        g_set_error(error,
                    OSINFO_MEDIA_ERROR,
                    OSINFO_MEDIA_ERROR_NOT_BOOTABLE,
                    "Install media is not bootable");

        goto EXIT;
    }

    uri = g_file_get_uri(file);
    ret = g_object_new(OSINFO_TYPE_MEDIA,
                       "id", uri,
                       NULL);
    osinfo_entity_set_param(OSINFO_ENTITY(ret),
                            OSINFO_MEDIA_PROP_URL,
                            uri);
    g_free(uri);
    if (pvd.volume[0] != 0)
        osinfo_entity_set_param(OSINFO_ENTITY(ret),
                                OSINFO_MEDIA_PROP_VOLUME_ID,
                                pvd.volume);
    if (pvd.system[0] != 0)
        osinfo_entity_set_param(OSINFO_ENTITY(ret),
                                OSINFO_MEDIA_PROP_SYSTEM_ID,
                                pvd.system);
    if (pvd.publisher[0] != 0)
        osinfo_entity_set_param(OSINFO_ENTITY(ret),
                                OSINFO_MEDIA_PROP_PUBLISHER_ID,
                                pvd.publisher);

EXIT:
    g_object_unref(stream);
    g_object_unref(file);

    return ret;
}

/**
 * osinfo_entity_get_architecture:
 * @media: a #OsinfoMedia instance
 *
 * Retrieves the target hardware architecture of the OS @media provides.
 *
 * Returns: (transfer none): the hardware architecture, or NULL
 */
const gchar *osinfo_media_get_architecture(OsinfoMedia *media)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(media),
                                         OSINFO_MEDIA_PROP_ARCHITECTURE);
}

/**
 * osinfo_entity_get_url:
 * @media: a #OsinfoMedia instance
 *
 * The URL to the @media
 *
 * Returns: (transfer none): the URL, or NULL
 */
const gchar *osinfo_media_get_url(OsinfoMedia *media)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(media),
                                         OSINFO_MEDIA_PROP_URL);
}

/**
 * osinfo_entity_get_volume_id:
 * @media: a #OsinfoMedia instance
 *
 * If @media is an ISO9660 image/device, this function retrieves the expected
 * volume ID.
 *
 * Note: In practice, this will usually not be an exact match to the volume ID
 * on the ISO image/device but rather a substring of it.
 *
 * Returns: (transfer none): the volume id, or NULL
 */
const gchar *osinfo_media_get_volume_id(OsinfoMedia *media)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(media),
                                         OSINFO_MEDIA_PROP_VOLUME_ID);
}

/**
 * osinfo_entity_get_system_id:
 * @media: a #OsinfoMedia instance
 *
 * If @media is an ISO9660 image/device, this function retrieves the expected
 * system ID.
 *
 * Note: In practice, this will usually not be an exact match to the system ID
 * on the ISO image/device but rather a substring of it.
 *
 * Returns: (transfer none): the system id, or NULL
 */
const gchar *osinfo_media_get_system_id(OsinfoMedia *media)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(media),
                                         OSINFO_MEDIA_PROP_SYSTEM_ID);
}

/**
 * osinfo_entity_get_publisher_id:
 * @media: a #OsinfoMedia instance
 *
 * If @media is an ISO9660 image/device, this function retrieves the expected
 * publisher ID.
 *
 * Note: In practice, this will usually not be an exact match to the system ID
 * on the ISO image/device but rather a substring of it.
 *
 * Returns: (transfer none): the publisher id, or NULL
 */
const gchar *osinfo_media_get_publisher_id(OsinfoMedia *media)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(media),
                                         OSINFO_MEDIA_PROP_PUBLISHER_ID);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
