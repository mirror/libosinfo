#include <stdlib.h>
#include <osinfo/osinfo.h>
#include <check.h>

struct ISOInfo {
    gchar *shortid;
    gchar *filename;
    OsinfoMedia *media;
};

static void free_iso(struct ISOInfo *info)
{
    if (!info)
        return;

    g_free(info->filename);
    g_free(info->shortid);
    if (info->media)
        g_object_unref(info->media);
    g_free(info);
}


static struct ISOInfo *load_iso(GFile *file, const gchar *shortid, const gchar *name, GError **error)
{
    struct ISOInfo *info = g_new0(struct ISOInfo, 1);
    GFileInputStream *fis = NULL;
    GDataInputStream *dis = NULL;
    gchar *line;
    const gchar *arch;

    if (!(fis = g_file_read(file, NULL, error)))
        goto error;

    if (!(dis = g_data_input_stream_new(G_INPUT_STREAM(fis))))
        goto error;

    info->filename = g_strdup(name);
    info->shortid = g_strdup(shortid);
    if (strstr(name, "amd64") ||
             strstr(name, "x64") ||
             strstr(name, "x86_64"))
        arch = "x86_64";
    else if (strstr(name, "i386") ||
             strstr(name, "i686") ||
             strstr(name, "x86"))
        arch = "i386";
    else if (strstr(name, "ppc"))
        arch = "ppc";
    else {
        g_warning("Unknown arch in %s", name);
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
        }
    }

    if (*error)
        goto error;

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
    gchar *cwd = g_get_current_dir();
    gchar *path = g_strdup_printf("%s/isodata/%s", cwd, vendor);
    GList *ret = NULL;
    GFile *f = g_file_new_for_path(path);


    ret = load_distros(f, error);

    g_object_unref(f);
    g_free(cwd);
    g_free(path);
    return ret;
}


START_TEST(test_fedora)
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

    isos = load_isos("fedora", &error);

    fail_unless(isos != NULL, "ISOs must not be NULL");

    tmp = isos;
    while (tmp) {
        struct ISOInfo *info  = tmp->data;
        OsinfoMedia *media = NULL;
        OsinfoOs *os = osinfo_db_guess_os_from_media(db,
                                                     info->media,
                                                     &media);

        fail_unless(os != NULL, "ISO %s matched OS %s",
                    info->filename, info->shortid);

        const gchar *shortid = osinfo_product_get_short_id(OSINFO_PRODUCT(os));
        fail_unless(g_str_equal(shortid, info->shortid),
                    "ISO %s matched OS %s, not %s",
                    info->filename, info->shortid, shortid);

        tmp = tmp->next;
    }

    g_list_foreach(isos, (GFunc)free_iso, NULL);
    g_list_free(isos);

    g_object_unref(db);
}
END_TEST



START_TEST(test_windows)
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

    isos = load_isos("windows", &error);

    fail_unless(isos != NULL, "ISOs must not be NULL %s", error ? error->message : "unknown");

    tmp = isos;
    while (tmp) {
        struct ISOInfo *info  = tmp->data;
        OsinfoMedia *media = NULL;
        OsinfoOs *os = osinfo_db_guess_os_from_media(db,
                                                     info->media,
                                                     &media);

        fail_unless(os != NULL, "ISO %s matched OS %s",
                    info->filename, info->shortid);

        const gchar *shortid = osinfo_product_get_short_id(OSINFO_PRODUCT(os));
        fail_unless(g_str_equal(shortid, info->shortid),
                    "ISO %s matched OS %s, not %s",
                    info->filename, info->shortid, shortid);

        tmp = tmp->next;
    }

    g_list_foreach(isos, (GFunc)free_iso, NULL);
    g_list_free(isos);

    g_object_unref(db);
}
END_TEST



static Suite *
list_suite(void)
{
    Suite *s = suite_create("List");
    TCase *tc = tcase_create("Core");
    tcase_set_timeout(tc, 20);

    tcase_add_test(tc, test_fedora);
    tcase_add_test(tc, test_windows);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = list_suite ();
    SRunner *sr = srunner_create (s);

    g_type_init();

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

    srunner_run_all (sr, CK_ENV);
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
