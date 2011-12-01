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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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

typedef struct _CreateFromLocationAsyncData CreateFromLocationAsyncData;
struct _CreateFromLocationAsyncData {
    GFile *file;

    gint priority;
    GCancellable *cancellable;

    GSimpleAsyncResult *res;

    PrimaryVolumeDescriptor pvd;
    SupplementaryVolumeDescriptor svd;
};

static void create_from_location_async_data_free
                                (CreateFromLocationAsyncData *data)
{
   g_object_unref(data->file);
   g_clear_object(&data->cancellable);
   g_object_unref(data->res);

   g_slice_free(CreateFromLocationAsyncData, data);
}

typedef struct _CreateFromLocationData CreateFromLocationData;
struct _CreateFromLocationData {
    GMainLoop *main_loop;

    GAsyncResult *res;
};

static void create_from_location_data_free(CreateFromLocationData *data)
{
   g_object_unref(data->res);
   g_main_loop_unref(data->main_loop);

   g_slice_free(CreateFromLocationData, data);
}

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

enum {
    PROP_0,

    PROP_ARCHITECTURE,
    PROP_URL,
    PROP_VOLUME_ID,
    PROP_PUBLISHER_ID,
    PROP_SYSTEM_ID,
    PROP_KERNEL_PATH,
    PROP_INITRD_PATH,
    PROP_INSTALLER,
    PROP_LIVE,
};

static void
osinfo_media_get_property (GObject    *object,
                               guint       property_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
    OsinfoMedia *media = OSINFO_MEDIA (object);

    switch (property_id) {
    case PROP_ARCHITECTURE:
        g_value_set_string (value,
                            osinfo_media_get_architecture (media));
        break;

    case PROP_URL:
        g_value_set_string (value,
                            osinfo_media_get_url (media));
        break;

    case PROP_VOLUME_ID:
        g_value_set_string (value,
                            osinfo_media_get_volume_id (media));
        break;

    case PROP_PUBLISHER_ID:
        g_value_set_string (value,
                            osinfo_media_get_publisher_id (media));
        break;

    case PROP_SYSTEM_ID:
        g_value_set_string (value,
                            osinfo_media_get_system_id (media));
        break;

    case PROP_KERNEL_PATH:
        g_value_set_string (value,
                            osinfo_media_get_kernel_path (media));
        break;

    case PROP_INITRD_PATH:
        g_value_set_string (value,
                            osinfo_media_get_initrd_path (media));
        break;

    case PROP_INSTALLER:
        g_value_set_boolean (value,
                             osinfo_media_get_installer (media));
        break;

    case PROP_LIVE:
        g_value_set_boolean (value,
                             osinfo_media_get_live (media));
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void set_param_from_boolean (OsinfoMedia *media,
                                    const gchar *key,
                                    gboolean value)
{
    if (value)
        osinfo_entity_set_param (OSINFO_ENTITY(media), key, "true");
    else
        osinfo_entity_set_param (OSINFO_ENTITY(media), key, "false");
}


static void
osinfo_media_set_property(GObject      *object,
                              guint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
    OsinfoMedia *media = OSINFO_MEDIA (object);

    switch (property_id) {
    case PROP_ARCHITECTURE:
        osinfo_entity_set_param (OSINFO_ENTITY(media),
                                 OSINFO_MEDIA_PROP_ARCHITECTURE,
                                 g_value_get_string (value));
        break;

    case PROP_URL:
        osinfo_entity_set_param (OSINFO_ENTITY(media),
                                 OSINFO_MEDIA_PROP_URL,
                                 g_value_get_string (value));
        break;

    case PROP_VOLUME_ID:
        osinfo_entity_set_param (OSINFO_ENTITY(media),
                                 OSINFO_MEDIA_PROP_VOLUME_ID,
                                 g_value_get_string (value));
        break;

    case PROP_PUBLISHER_ID:
        osinfo_entity_set_param (OSINFO_ENTITY(media),
                                 OSINFO_MEDIA_PROP_PUBLISHER_ID,
                                 g_value_get_string (value));
        break;

    case PROP_SYSTEM_ID:
        osinfo_entity_set_param (OSINFO_ENTITY(media),
                                 OSINFO_MEDIA_PROP_SYSTEM_ID,
                                 g_value_get_string (value));
        break;

    case PROP_KERNEL_PATH:
        osinfo_entity_set_param (OSINFO_ENTITY(media),
                                 OSINFO_MEDIA_PROP_KERNEL,
                                 g_value_get_string (value));
        break;

    case PROP_INITRD_PATH:
        osinfo_entity_set_param (OSINFO_ENTITY(media),
                                 OSINFO_MEDIA_PROP_INITRD,
                                 g_value_get_string (value));
        break;

    case PROP_LIVE:
        set_param_from_boolean (media,
                                OSINFO_MEDIA_PROP_LIVE,
                                g_value_get_boolean (value));
        break;

    case PROP_INSTALLER:
        set_param_from_boolean (media,
                                OSINFO_MEDIA_PROP_INSTALLER,
                                g_value_get_boolean (value));
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

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
    GParamSpec *pspec;

    g_klass->finalize = osinfo_media_finalize;
    g_klass->get_property = osinfo_media_get_property;
    g_klass->set_property = osinfo_media_set_property;
    g_type_class_add_private (klass, sizeof (OsinfoMediaPrivate));

    /**
     * OsinfoMedia::architecture:
     *
     * The target hardware architecture of this media.
     */
    pspec = g_param_spec_string ("architecture",
                                 "ARCHITECTURE",
                                 "CPU Architecture",
                                 NULL /* default value */,
                                 G_PARAM_READWRITE |
                                 G_PARAM_STATIC_NAME |
                                 G_PARAM_STATIC_NICK |
                                 G_PARAM_STATIC_BLURB);
    g_object_class_install_property (g_klass, PROP_ARCHITECTURE, pspec);

    /**
     * OsinfoMedia::url
     *
     * The URL to this media.
     */
    pspec = g_param_spec_string ("url",
                                 "URL",
                                 "The URL to this media",
                                 NULL /* default value */,
                                 G_PARAM_READWRITE |
                                 G_PARAM_STATIC_NAME |
                                 G_PARAM_STATIC_NICK |
                                 G_PARAM_STATIC_BLURB);
    g_object_class_install_property (g_klass, PROP_URL, pspec);

    /**
     * OsinfoMedia::volume-id
     *
     * Expected volume ID (regular expression) for ISO9660 image/device.
     */
    pspec = g_param_spec_string ("volume-id",
                                 "VolumeID",
                                 "Expected ISO9660 volume ID",
                                 NULL /* default value */,
                                 G_PARAM_READWRITE |
                                 G_PARAM_STATIC_NAME |
                                 G_PARAM_STATIC_NICK |
                                 G_PARAM_STATIC_BLURB);
    g_object_class_install_property (g_klass, PROP_VOLUME_ID, pspec);

    /**
     * OsinfoMedia::publisher-id
     *
     * Expected publisher ID (regular expression) for ISO9660 image/device.
     */
    pspec = g_param_spec_string ("publisher-id",
                                 "PublisherID",
                                 "Expected ISO9660 publisher ID",
                                 NULL /* default value */,
                                 G_PARAM_READWRITE |
                                 G_PARAM_STATIC_NAME |
                                 G_PARAM_STATIC_NICK |
                                 G_PARAM_STATIC_BLURB);
    g_object_class_install_property (g_klass, PROP_PUBLISHER_ID, pspec);

    /**
     * OsinfoMedia::system-id
     *
     * Expected system ID (regular expression) for ISO9660 image/device.
     */
    pspec = g_param_spec_string ("system-id",
                                 "SystemID",
                                 "Expected ISO9660 system ID",
                                 NULL /* default value */,
                                 G_PARAM_READWRITE |
                                 G_PARAM_STATIC_NAME |
                                 G_PARAM_STATIC_NICK |
                                 G_PARAM_STATIC_BLURB);
    g_object_class_install_property (g_klass, PROP_SYSTEM_ID, pspec);

    /**
     * OsinfoMedia::kernel-path
     *
     * The path to the kernel image in the install tree.
     */
    pspec = g_param_spec_string ("kernel-path",
                                 "KernelPath",
                                 "The path to the kernel image",
                                 NULL /* default value */,
                                 G_PARAM_READWRITE |
                                 G_PARAM_STATIC_NAME |
                                 G_PARAM_STATIC_NICK |
                                 G_PARAM_STATIC_BLURB);
    g_object_class_install_property (g_klass, PROP_KERNEL_PATH, pspec);

    /**
     * OsinfoMedia::initrd-path
     *
     * The path to the initrd image in the install tree.
     */
    pspec = g_param_spec_string ("initrd-path",
                                 "InitrdPath",
                                 "The path to the inirtd image",
                                 NULL /* default value */,
                                 G_PARAM_READWRITE |
                                 G_PARAM_STATIC_NAME |
                                 G_PARAM_STATIC_NICK |
                                 G_PARAM_STATIC_BLURB);
    g_object_class_install_property (g_klass, PROP_INITRD_PATH, pspec);

    /**
     * OsinfoMedia::installer
     *
     * Whether media provides a installer for an OS.
     */
    pspec = g_param_spec_boolean ("installer",
                                  "Installer",
                                  "Media provides a installer",
                                  TRUE /* default value */,
                                  G_PARAM_READWRITE |
                                  G_PARAM_CONSTRUCT | /* to set default value */
                                  G_PARAM_STATIC_NAME |
                                  G_PARAM_STATIC_NICK |
                                  G_PARAM_STATIC_BLURB);
    g_object_class_install_property (g_klass, PROP_INSTALLER, pspec);

    /**
     * OsinfoMedia::live
     *
     * Whether media can boot directly an OS without any installations.
     */
    pspec = g_param_spec_boolean ("live",
                                  "Live",
                                  "Media can boot directly w/o installation",
                                  FALSE /* default value */,
                                  G_PARAM_READWRITE |
                                  G_PARAM_CONSTRUCT | /* to set default value */
                                  G_PARAM_STATIC_NAME |
                                  G_PARAM_STATIC_NICK |
                                  G_PARAM_STATIC_BLURB);
    g_object_class_install_property (g_klass, PROP_LIVE, pspec);
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

static void on_media_create_from_location_ready (GObject *source_object,
                                                 GAsyncResult *res,
                                                 gpointer user_data)
{
    CreateFromLocationData *data = (CreateFromLocationData *)user_data;

    data->res = g_object_ref(res);

    g_main_loop_quit(data->main_loop);
}

/**
 * osinfo_media_create_from_location:
 * @location: the location of an installation media
 * @cancellable: (allow-none): a #GCancellable, or %NULL
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
    CreateFromLocationData *data;
    OsinfoMedia *ret;

    data = g_slice_new0(CreateFromLocationData);
    data->main_loop = g_main_loop_new (g_main_context_get_thread_default (),
                                       TRUE);

    osinfo_media_create_from_location_async(location,
                                            G_PRIORITY_DEFAULT,
                                            cancellable,
                                            on_media_create_from_location_ready,
                                            data);

    /* Loop till we get a reply (or time out) */
    if (g_main_loop_is_running (data->main_loop))
        g_main_loop_run (data->main_loop);

    ret = osinfo_media_create_from_location_finish(data->res, error);
    create_from_location_data_free(data);

    return ret;
}

static gboolean is_str_empty(const gchar *str) {
    guint8 i;
    gboolean ret = TRUE;

    if ((str == NULL) || (*str == 0))
        return TRUE;

    for (i = 0; i < strlen (str); i++)
        if (!g_ascii_isspace (str[i])) {
           ret = FALSE;

           break;
        }

    return ret;
}

static void on_svd_read (GObject *source,
                         GAsyncResult *res,
                         gpointer user_data)
{
    OsinfoMedia *ret = NULL;
    GInputStream *stream = G_INPUT_STREAM(source);
    gchar *uri;
    GError *error = NULL;
    CreateFromLocationAsyncData *data;

    data = (CreateFromLocationAsyncData *)user_data;

    if (g_input_stream_read_finish(stream,
                                   res,
                                   &error) < sizeof(data->svd)) {
        if (error)
            g_prefix_error(&error,
                           "Failed to read supplementary volume descriptor");
        else
            g_set_error(&error,
                        OSINFO_MEDIA_ERROR,
                        OSINFO_MEDIA_ERROR_NO_SVD,
                        "Supplementary volume descriptor unavailable");


        goto EXIT;
    }

    data->svd.system[MAX_SYSTEM - 1] = 0;

    if (strncmp(BOOTABLE_TAG, data->svd.system, sizeof(BOOTABLE_TAG) != 0)) {
        g_set_error(&error,
                    OSINFO_MEDIA_ERROR,
                    OSINFO_MEDIA_ERROR_NOT_BOOTABLE,
                    "Install media is not bootable");

        goto EXIT;
    }

    uri = g_file_get_uri(data->file);
    ret = g_object_new(OSINFO_TYPE_MEDIA,
                       "id", uri,
                       NULL);
    osinfo_entity_set_param(OSINFO_ENTITY(ret),
                            OSINFO_MEDIA_PROP_URL,
                            uri);
    g_free(uri);
    if (!is_str_empty (data->pvd.volume))
        osinfo_entity_set_param(OSINFO_ENTITY(ret),
                                OSINFO_MEDIA_PROP_VOLUME_ID,
                                data->pvd.volume);
    if (!is_str_empty (data->pvd.system))
        osinfo_entity_set_param(OSINFO_ENTITY(ret),
                                OSINFO_MEDIA_PROP_SYSTEM_ID,
                                data->pvd.system);
    if (!is_str_empty (data->pvd.publisher))
        osinfo_entity_set_param(OSINFO_ENTITY(ret),
                                OSINFO_MEDIA_PROP_PUBLISHER_ID,
                                data->pvd.publisher);

EXIT:
    if (error != NULL)
        g_simple_async_result_take_error(data->res, error);
    else
        g_simple_async_result_set_op_res_gpointer(data->res, ret, NULL);
    g_simple_async_result_complete (data->res);

    g_object_unref(stream);
    create_from_location_async_data_free(data);
}

static void on_pvd_read (GObject *source,
                         GAsyncResult *res,
                         gpointer user_data)
{
    GInputStream *stream = G_INPUT_STREAM(source);
    CreateFromLocationAsyncData *data;
    GError *error = NULL;

    data = (CreateFromLocationAsyncData *)user_data;

    if (g_input_stream_read_finish(stream,
                                   res,
                                   &error) < sizeof(data->pvd)) {
        if (error)
            g_prefix_error(&error, "Failed to read primary volume descriptor");
        else
            g_set_error(&error,
                        OSINFO_MEDIA_ERROR,
                        OSINFO_MEDIA_ERROR_NO_PVD,
                        "Primary volume descriptor unavailable");

        goto ON_ERROR;
    }

    data->pvd.volume[MAX_VOLUME - 1] = 0;
    data->pvd.system[MAX_SYSTEM - 1] = 0;
    data->pvd.publisher[MAX_PUBLISHER - 1] = 0;

    if (is_str_empty(data->pvd.volume)) {
        g_set_error(&error,
                    OSINFO_MEDIA_ERROR,
                    OSINFO_MEDIA_ERROR_INSUFFICIENT_METADATA,
                    "Insufficient metadata on installation media");

        goto ON_ERROR;
    }

    g_input_stream_read_async(stream,
                              &data->svd,
                              sizeof(data->svd),
                              data->priority,
                              data->cancellable,
                              on_svd_read,
                              data);
    return;

ON_ERROR:
    g_simple_async_result_take_error(data->res, error);
    g_simple_async_result_complete (data->res);
    create_from_location_async_data_free(data);
}

static void on_location_skipped(GObject *source,
                                GAsyncResult *res,
                                gpointer user_data)
{
    GInputStream *stream = G_INPUT_STREAM(source);
    CreateFromLocationAsyncData *data;
    GError *error = NULL;

    data = (CreateFromLocationAsyncData *)user_data;

    if (g_input_stream_skip_finish(stream, res, &error) < PVD_OFFSET) {
        if (error)
            g_prefix_error(&error, "Failed to skip %d bytes", PVD_OFFSET);
        else
            g_set_error(&error,
                         OSINFO_MEDIA_ERROR,
                         OSINFO_MEDIA_ERROR_NO_DESCRIPTORS,
                         "No volume descriptors");
        g_simple_async_result_take_error(data->res, error);
        g_simple_async_result_complete (data->res);
        create_from_location_async_data_free(data);

        return;
    }

    g_input_stream_read_async(stream,
                              &data->pvd,
                              sizeof(data->pvd),
                              data->priority,
                              data->cancellable,
                              on_pvd_read,
                              data);
}

static void on_location_read(GObject *source,
                             GAsyncResult *res,
                             gpointer user_data)
{
    GFileInputStream *stream;
    CreateFromLocationAsyncData *data;
    GError *error = NULL;

    data = (CreateFromLocationAsyncData *)user_data;

    stream = g_file_read_finish(G_FILE(source), res, &error);
    if (error != NULL) {
        g_prefix_error(&error, "Failed to open file");
        g_simple_async_result_take_error(data->res, error);
        g_simple_async_result_complete (data->res);
        create_from_location_async_data_free(data);

        return;
    }

    g_input_stream_skip_async(G_INPUT_STREAM(stream),
                              PVD_OFFSET,
                              data->priority,
                              data->cancellable,
                              on_location_skipped,
                              data);
}

/**
 * osinfo_media_create_from_location_async:
 * @location: the location of an installation media
 * @priority: the I/O priority of the request
 * @cancellable: (allow-none): a #GCancellable, or %NULL
 * @callback: Function to call when result of this call is ready
 * @user_data: The user data to pass to @callback, or %NULL
 *
 * Asynchronous variant of #osinfo_media_create_from_location.
 */
void osinfo_media_create_from_location_async(const gchar *location,
                                             gint priority,
                                             GCancellable *cancellable,
                                             GAsyncReadyCallback callback,
                                             gpointer user_data)
{
    CreateFromLocationAsyncData *data;

    g_return_if_fail(location != NULL);

    data = g_slice_new0(CreateFromLocationAsyncData);
    data->res = g_simple_async_result_new
                                (NULL,
                                 callback,
                                 user_data,
                                 osinfo_media_create_from_location_async);
    data->file = g_file_new_for_commandline_arg(location);
    data->priority = priority;
    data->cancellable = cancellable;
    g_file_read_async(data->file,
                      priority,
                      cancellable,
                      on_location_read,
                      data);
}

/**
 * osinfo_media_create_from_location_finish:
 * @res: a #GAsyncResult
 * @error: The location where to store any error, or %NULL
 *
 * Finishes an asynchronous media object creation process started with
 * #osinfo_media_create_from_location_async.
 *
 * Returns: (transfer full): a new #OsinfoMedia , or NULL on error
 */
OsinfoMedia *osinfo_media_create_from_location_finish(GAsyncResult *res,
                                                      GError **error)
{
    GSimpleAsyncResult *simple = G_SIMPLE_ASYNC_RESULT(res);

    g_return_val_if_fail(error == NULL || *error == NULL, NULL);

    if (g_simple_async_result_propagate_error(simple, error))
        return NULL;

    return g_simple_async_result_get_op_res_gpointer(simple);
}

/**
 * osinfo_media_get_architecture:
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
 * osinfo_media_get_url:
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
 * osinfo_media_get_volume_id:
 * @media: a #OsinfoMedia instance
 *
 * If @media is an ISO9660 image/device, this function retrieves the expected
 * volume ID.
 *
 * Note: In practice, this will usually not be the exact copy of the volume ID
 * string on the ISO image/device but rather a regular expression that matches
 * it.
 *
 * Returns: (transfer none): the volume id, or NULL
 */
const gchar *osinfo_media_get_volume_id(OsinfoMedia *media)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(media),
                                         OSINFO_MEDIA_PROP_VOLUME_ID);
}

/**
 * osinfo_media_get_system_id:
 * @media: a #OsinfoMedia instance
 *
 * If @media is an ISO9660 image/device, this function retrieves the expected
 * system ID.
 *
 * Note: In practice, this will usually not be the exact copy of the system ID
 * string on the ISO image/device but rather a regular expression that matches
 * it.
 *
 * Returns: (transfer none): the system id, or NULL
 */
const gchar *osinfo_media_get_system_id(OsinfoMedia *media)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(media),
                                         OSINFO_MEDIA_PROP_SYSTEM_ID);
}

/**
 * osinfo_media_get_publisher_id:
 * @media: a #OsinfoMedia instance
 *
 * If @media is an ISO9660 image/device, this function retrieves the expected
 * publisher ID.
 *
 * Note: In practice, this will usually not be the exact copy of the publisher
 * ID string on the ISO image/device but rather a regular expression that
 * matches it.
 *
 * Returns: (transfer none): the publisher id, or NULL
 */
const gchar *osinfo_media_get_publisher_id(OsinfoMedia *media)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(media),
                                         OSINFO_MEDIA_PROP_PUBLISHER_ID);
}

/**
 * osinfo_media_get_kernel_path:
 * @media: a #OsinfoMedia instance
 *
 * Retrieves the path to the kernel image in the install tree.
 *
 * Note: This only applies to installer medias of 'Linux' OS family.
 *
 * Returns: (transfer none): the path to kernel image, or NULL
 */
const gchar *osinfo_media_get_kernel_path(OsinfoMedia *media)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(media),
                                         OSINFO_MEDIA_PROP_KERNEL);
}

/**
 * osinfo_media_get_initrd_path:
 * @media: a #OsinfoMedia instance
 *
 * Retrieves the path to the initrd image in the install tree.
 *
 * Note: This only applies to installer medias of 'Linux' OS family.
 *
 * Returns: (transfer none): the path to initrd image, or NULL
 */
const gchar *osinfo_media_get_initrd_path(OsinfoMedia *media)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(media),
                                         OSINFO_MEDIA_PROP_INITRD);
}

static gboolean get_param_as_bool (OsinfoMedia *media,
                                   const char *key,
                                   gboolean default_value)
{
    const gchar *value;

    value = osinfo_entity_get_param_value(OSINFO_ENTITY(media), key);
    if (value == NULL)
        return default_value;

    return (g_strcmp0 ("true", value) == 0 ||
            g_strcmp0 ("yes", value) == 0);
}

/**
 * osinfo_media_get_installer:
 * @media: a #OsinfoMedia instance
 *
 * Whether @media provides a installer for an OS.
 *
 * Returns: #TRUE if media is installer, #FALSE otherwise
 */
gboolean osinfo_media_get_installer(OsinfoMedia *media)
{
    return get_param_as_bool (media, OSINFO_MEDIA_PROP_INSTALLER, TRUE);
}

/**
 * osinfo_media_get_live:
 * @media: a #OsinfoMedia instance
 *
 * Whether @media can boot directly an OS without any installations.
 *
 * Returns: #TRUE if media is live, #FALSE otherwise
 */
gboolean osinfo_media_get_live(OsinfoMedia *media)
{
    return get_param_as_bool (media, OSINFO_MEDIA_PROP_LIVE, FALSE);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
