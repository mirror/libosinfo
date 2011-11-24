/*
 * Copyright (C) 2011 Red Hat, Inc
 *
 * osinfo-detect: Given a path to a ISO9660 image/device, detects if media is
 *                bootable and the relavent OS if media is an installer for it.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Authors:
 *   Zeeshan Ali <zeenix@redhat.com>
 */

#include <osinfo/osinfo.h>
#include <string.h>

#define FORMAT_STR_PLAIN "plain"
#define FORMAT_STR_ENV "env"

typedef enum {
    OUTPUT_FORMAT_PLAIN,
    OUTPUT_FORMAT_ENV
} OutputFormat;

static OutputFormat format = OUTPUT_FORMAT_PLAIN;

static gboolean parse_format_str(const gchar *option_name,
                                 const gchar *value,
                                 gpointer data,
                                 GError **error)
{
    if (strcmp(value, FORMAT_STR_ENV) == 0)
        format = OUTPUT_FORMAT_ENV;
    else if (strcmp(value, FORMAT_STR_PLAIN) == 0)
        format = OUTPUT_FORMAT_PLAIN;
    else {
        g_set_error(error,
                    G_OPTION_ERROR,
                    G_OPTION_ERROR_FAILED,
                    "Invalid value '%s'", value);

        return FALSE;
    }

    return TRUE;
}

static GOptionEntry entries[] =
{
    { "format", 'f', 0,
      G_OPTION_ARG_CALLBACK, parse_format_str,
      "Output format. Default: plain",
      "plain|env." },
    { NULL }
};

static void print_bootable(gboolean bootable)
{
    if (bootable)
        if (format == OUTPUT_FORMAT_ENV)
            g_print("OSINFO_BOOTABLE=1\n");
        else
            g_print("Media is bootable.\n");
    else
        if (format == OUTPUT_FORMAT_ENV)
            g_print("OSINFO_BOOTABLE=0\n");
        else
            g_print("Media is not bootable.\n");
}

static void print_os(OsinfoOs *os, OsinfoMedia *media)
{
    if (os == NULL)
        return;

    if (format == OUTPUT_FORMAT_ENV) {
        const gchar *id = osinfo_entity_get_id(OSINFO_ENTITY(os));

        if (osinfo_media_get_installer (media))
            g_print("OSINFO_INSTALLER=%s\n", id);
        if (osinfo_media_get_live (media))
            g_print("OSINFO_LIVE=%s\n", id);
        g_print("OSINFO_MEDIA=%s\n",
                osinfo_entity_get_id(OSINFO_ENTITY(media)));
    } else {
        const gchar *name = osinfo_product_get_name(OSINFO_PRODUCT(os));

        if (osinfo_media_get_installer (media))
            g_print("Media is an installer for OS '%s'\n", name);
        if (osinfo_media_get_live (media))
            g_print("Media is live media for OS '%s'\n", name);
    }
}

gint main(gint argc, gchar **argv)
{
    GOptionContext *context;
    GError *error = NULL;
    OsinfoMedia *media = NULL;
    OsinfoMedia *matched_media = NULL;
    OsinfoLoader *loader = NULL;
    OsinfoDb *db = NULL;
    OsinfoOs *os = NULL;
    gint ret = 0;

    context = g_option_context_new("- Detect if media is bootable " \
                                   "and the relavent OS and distribution.");
    /* FIXME: We don't have a gettext package to pass to this function. */
    g_option_context_add_main_entries(context, entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_printerr("Error while parsing options: %s\n", error->message);
        g_printerr("%s\n", g_option_context_get_help(context, FALSE, NULL));

        ret = -1;
        goto EXIT;
    }

    if (argc < 2) {
        g_printerr("%s\n", g_option_context_get_help(context, FALSE, NULL));

        ret = -2;
        goto EXIT;
    }

    g_type_init();

    media = osinfo_media_create_from_location(argv[1], NULL, &error);
    if (error != NULL) {
        if (error->code != OSINFO_MEDIA_ERROR_NOT_BOOTABLE) {
            g_printerr("Error parsing media: %s\n", error->message);

            ret = -3;
            goto EXIT;
        } else
            print_bootable(FALSE);
    } else
        print_bootable(TRUE);

    loader = osinfo_loader_new();
    osinfo_loader_process_default_path(loader, &error);
    if (error != NULL) {
        g_printerr("Error loading OS data: %s\n", error->message);

        ret = -4;
        goto EXIT;
    }

    db = osinfo_loader_get_db(loader);
    os = osinfo_db_guess_os_from_media(db, media, &matched_media);

    print_os(os, matched_media);

EXIT:
    g_clear_error(&error);
    g_clear_object(&loader);
    g_option_context_free(context);

    return ret;
}
