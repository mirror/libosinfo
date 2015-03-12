/*
 * Copyright (C) 2009-2012, 2014 Red Hat, Inc.
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
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <config.h>

#include <stdlib.h>
#include <osinfo/osinfo.h>
#include <check.h>

struct ISOInfo {
    gchar *shortid;
    gchar *filename;
    OsinfoMedia *media;
    GHashTable *langs;
};

static void free_iso(struct ISOInfo *info)
{
    if (!info)
        return;

    g_free(info->filename);
    g_free(info->shortid);
    if (info->media)
        g_object_unref(info->media);
    if (info->langs)
        g_hash_table_unref(info->langs);
    g_free(info);
}

static gboolean load_langs(GFile *file, struct ISOInfo *info, GError **error)
{
    char *path;
    GKeyFile *lang_file;
    GStrv langs;
    GStrv it;

    path = g_file_get_path(file);
    if (path == NULL)
        return FALSE;
    if (!g_str_has_suffix(path, ".txt")) {
        g_free(path);
        return FALSE;
    }
    strcpy(&path[strlen(path) - sizeof(".txt") + 1], ".lng");
    lang_file = g_key_file_new();
    if (!g_key_file_load_from_file(lang_file, path, G_KEY_FILE_NONE, NULL)) {
        g_key_file_free(lang_file);
        g_free(path);
        return FALSE;
    }
    langs = g_key_file_get_string_list(lang_file, "general", "l10n-language",
                                       NULL, NULL);
    if (langs == NULL) {
        g_key_file_free(lang_file);
        g_free(path);
        return FALSE;
    }

    for (it = langs; (it != NULL) && (*it != NULL); it++) {
        g_hash_table_add(info->langs, g_strdup(*it));
    }

    g_strfreev(langs);
    g_key_file_free(lang_file);
    g_free(path);

    return TRUE;
}

static struct ISOInfo *load_iso(GFile *file, const gchar *shortid, const gchar *name, GError **error)
{
    struct ISOInfo *info = g_new0(struct ISOInfo, 1);
    GFileInputStream *fis = NULL;
    GDataInputStream *dis = NULL;
    gchar *line;
    const gchar *arch;
    gint64 vol_size = -1, blk_size;

    if (!(fis = g_file_read(file, NULL, error)))
        goto error;

    if (!(dis = g_data_input_stream_new(G_INPUT_STREAM(fis))))
        goto error;

    info->filename = g_strdup(name);
    info->shortid = g_strdup(shortid);
    info->langs = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    if (strstr(name, "amd64") ||
             strstr(name, "x64") ||
             strstr(name, "x86_64") ||
             strstr(name, "64bit") ||
             strstr(name, "64-bit"))
        arch = "x86_64";
    else if (strstr(name, "i386") ||
             strstr(name, "i686") ||
             strstr(name, "x86") ||
             strstr(name, "32bit") ||
             strstr(name, "32-bit"))
        arch = "i386";
    else if (strstr(name, "ppc") ||
             strstr(name, "powerpc"))
        arch = "ppc";
    else if (strstr(name, "hppa"))
        arch = "hppa";
    else if (strstr(name, "sparc"))
        arch = "sparc";
    else if (strstr(name, "ia64"))
        arch = "ia64";
    else {
        g_debug("Unknown arch in %s", name);
        arch = "i386";
    }

    info->media = osinfo_media_new(name, arch);

    while ((line = g_data_input_stream_read_line(dis, NULL, NULL, error)) != NULL) {
        if (g_str_has_prefix(line, "Volume id:")) {
            osinfo_entity_set_param(OSINFO_ENTITY(info->media),
                                    OSINFO_MEDIA_PROP_VOLUME_ID,
                                    line + 11);
        } else if (g_str_has_prefix(line, "Publisher id:")) {
            osinfo_entity_set_param(OSINFO_ENTITY(info->media),
                                    OSINFO_MEDIA_PROP_PUBLISHER_ID,
                                    line + 14);
        } else if (g_str_has_prefix(line, "System id:")) {
            osinfo_entity_set_param(OSINFO_ENTITY(info->media),
                                    OSINFO_MEDIA_PROP_SYSTEM_ID,
                                    line + 11);
        } else if (g_str_has_prefix(line, "Application id:")) {
            osinfo_entity_set_param(OSINFO_ENTITY(info->media),
                                    OSINFO_MEDIA_PROP_APPLICATION_ID,
                                    line + 16);
        } else if (g_str_has_prefix(line, "Logical block size is:")) {
            blk_size = (gint64) atoll (line + 23);
        } else if (g_str_has_prefix(line, "Volume size is:")) {
            vol_size = atoll (line + 16);
        }
    }

    if (vol_size > 0)
        osinfo_entity_set_param_int64(OSINFO_ENTITY(info->media),
                                      OSINFO_MEDIA_PROP_VOLUME_SIZE,
                                      vol_size * blk_size);

    if (*error)
        goto error;

    load_langs(file, info, error);

 cleanup:
    if (fis)
        g_object_unref(fis);

    return info;

 error:
    free_iso(info);
    info = NULL;
    goto cleanup;
}


static GList *load_distro(GFile *dir, const gchar *shortid, GError **error) {
    GFileEnumerator *children = g_file_enumerate_children(dir,
                                                          "standard::*",
                                                          G_FILE_QUERY_INFO_NONE,
                                                          NULL,
                                                          error);
    GFileInfo *childinfo;
    GList *ret = NULL;

    if (!children)
        return NULL;

    while ((childinfo = g_file_enumerator_next_file(children, NULL, error)) != NULL) {
        if (g_file_info_get_file_type(childinfo) !=
            G_FILE_TYPE_REGULAR)
            continue;

        if (!g_str_has_suffix(g_file_info_get_name(childinfo), ".txt"))
            continue;

        GFile *child = g_file_get_child(dir, g_file_info_get_name(childinfo));
        struct ISOInfo *iso = load_iso(child,
                                       shortid,
                                       g_file_info_get_name(childinfo),
                                       error);
        g_object_unref(child);

        if (!iso)
            goto error;

        ret = g_list_append(ret, iso);
    }

    if (*error)
        goto error;

 cleanup:
    g_object_unref(children);

    return ret;

 error:
    g_list_foreach(ret, (GFunc)free_iso, NULL);
    g_list_free(ret);
    ret = NULL;
    goto cleanup;
}


static GList *load_distros(GFile *dir, GError **error)
{
    GFileEnumerator *children = g_file_enumerate_children(dir,
                                                          "standard::*",
                                                          G_FILE_QUERY_INFO_NONE,
                                                          NULL,
                                                          error);
    GFileInfo *childinfo;
    GList *ret = NULL;

    if (!children)
        return NULL;

    while ((childinfo = g_file_enumerator_next_file(children, NULL, error)) != NULL) {
        if (g_file_info_get_file_type(childinfo) !=
            G_FILE_TYPE_DIRECTORY)
            continue;

        GFile *child = g_file_get_child(dir, g_file_info_get_name(childinfo));
        GList *isos = load_distro(child, g_file_info_get_name(childinfo), error);

        g_object_unref(child);

        if (!isos && *error)
            goto error;
        ret = g_list_concat(ret, isos);
    }

    if (*error)
        goto error;

 cleanup:
    g_object_unref(children);

    return ret;

 error:
    g_list_foreach(ret, (GFunc)free_iso, NULL);
    g_list_free(ret);
    ret = NULL;
    goto cleanup;
}


static GList *load_isos(const gchar *vendor, GError **error)
{
    gchar *path = g_strdup_printf(SRCDIR "/test/isodata/%s", vendor);
    GList *ret = NULL;
    GFile *f = g_file_new_for_path(path);

    ret = load_distros(f, error);

    g_object_unref(f);
    g_free(path);
    return ret;
}


static void test_langs(struct ISOInfo *info)
{
    GList *langs;
    GList *it;

    /* exit early if there was no associated .lng file */
    if (g_hash_table_size(info->langs) == 0)
        return;

    langs = osinfo_media_get_languages(info->media);

    for (it = langs; it != NULL; it = it->next) {
        fail_unless(g_hash_table_contains(info->langs, it->data),
                    "%s not a known language for ISO %s",
                    it->data, info->filename);
        g_hash_table_remove(info->langs, it->data);
    }
    fail_unless(g_hash_table_size(info->langs) == 0,
                "some languages were not identified on ISO %s",
                info->filename);
}

static void test_one(const gchar *vendor)
{
    OsinfoLoader *loader = osinfo_loader_new();
    OsinfoDb *db = osinfo_loader_get_db(loader);
    GList *isos = NULL;
    GList *tmp;
    GError *error = NULL;

    fail_unless(OSINFO_IS_LOADER(loader), "Loader is not a LOADER");
    fail_unless(OSINFO_IS_DB(db), "Db is not a DB");

    osinfo_loader_process_path(loader, SRCDIR "/data", &error);
    fail_unless(error == NULL, error ? error->message : "none");

    isos = load_isos(vendor, &error);

    fail_unless(isos != NULL, "ISOs must not be NULL");

    tmp = isos;
    while (tmp) {
        struct ISOInfo *info  = tmp->data;
        gboolean matched = osinfo_db_identify_media(db, info->media);
        OsinfoOs *os;

        fail_unless(matched, "ISO %s was not matched by OS %s",
                    info->filename, info->shortid);

        g_object_get(info->media, "os", &os, NULL);
        const gchar *shortid = osinfo_product_get_short_id(OSINFO_PRODUCT(os));
        fail_unless(g_str_equal(shortid, info->shortid),
                    "ISO %s matched OS %s instead of expected %s",
                    info->filename, shortid, info->shortid);
        g_object_unref(G_OBJECT(os));
        test_langs(info);

        tmp = tmp->next;
    }

    g_list_foreach(isos, (GFunc)free_iso, NULL);
    g_list_free(isos);

    g_object_unref(loader);
}

START_TEST(test_fedora)
{
    test_one("fedora");
}
END_TEST

START_TEST(test_rhel)
{
    test_one("rhel");
}
END_TEST

START_TEST(test_ubuntu)
{
    test_one("ubuntu");
}
END_TEST

START_TEST(test_debian)
{
    test_one("debian");
}
END_TEST

START_TEST(test_windows)
{
    test_one("windows");
}
END_TEST

START_TEST(test_openbsd)
{
    test_one("openbsd");
}
END_TEST

START_TEST(test_opensuse)
{
    test_one("opensuse");
}
END_TEST

START_TEST(test_centos)
{
    test_one("centos");
}
END_TEST

START_TEST(test_gnome)
{
    test_one("gnome");
}
END_TEST

START_TEST(test_altlinux)
{
    test_one("altlinux");
}
END_TEST

START_TEST(test_mageia)
{
    test_one("mageia");
}
END_TEST

static Suite *
list_suite(void)
{
    Suite *s = suite_create("List");
    TCase *tc = tcase_create("Core");
    tcase_set_timeout(tc, 20);

    tcase_add_test(tc, test_fedora);
    tcase_add_test(tc, test_rhel);
    tcase_add_test(tc, test_ubuntu);
    tcase_add_test(tc, test_debian);
    tcase_add_test(tc, test_windows);
    tcase_add_test(tc, test_openbsd);
    tcase_add_test(tc, test_opensuse);
    tcase_add_test(tc, test_centos);
    tcase_add_test(tc, test_gnome);
    tcase_add_test(tc, test_altlinux);
    tcase_add_test(tc, test_mageia);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = list_suite();
    SRunner *sr = srunner_create(s);

#if !GLIB_CHECK_VERSION(2,35,1)
    g_type_init();
#endif

    /* Upfront so we don't confuse valgrind */
    osinfo_entity_get_type();
    osinfo_db_get_type();
    osinfo_device_get_type();
    osinfo_platform_get_type();
    osinfo_os_get_type();
    osinfo_list_get_type();
    osinfo_devicelist_get_type();
    osinfo_platformlist_get_type();
    osinfo_oslist_get_type();
    osinfo_filter_get_type();

    srunner_run_all(sr, CK_ENV);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
