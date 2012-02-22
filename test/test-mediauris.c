#include <config.h>
#include <stdlib.h>
#include <osinfo/osinfo.h>
#include <check.h>
#include <libsoup/soup.h>
#define HAVE_LIBSOUP_GNOME
#include <libsoup/soup-gnome.h>

static void test_media(OsinfoMediaList *medialist, GError **error, SoupSession *session)
{
    GList *mediael = NULL, *tmp;

    tmp = mediael = osinfo_list_get_elements(OSINFO_LIST(medialist));
    while (tmp) {
        OsinfoMedia *media = tmp->data;
        const gchar *url = osinfo_media_get_url(media);
        SoupMessage *msg;
        guint status;

        if (url == NULL || g_str_equal(url, "")) {
            tmp = tmp->next;
            continue;
        }

        g_print("%s\n", url);
        msg = soup_message_new("HEAD", url);
        status = soup_session_send_message(session, msg);

        fail_unless(SOUP_STATUS_IS_SUCCESSFUL(status), "Failed HEAD on %s", url);

        g_object_unref(msg);
        tmp = tmp->next;
    }

    g_list_free(mediael);
}

START_TEST(test_uris)
{
    SoupSession *session;
    OsinfoLoader *loader = osinfo_loader_new();
    OsinfoDb *db = osinfo_loader_get_db(loader);
    GError *error = NULL;
    OsinfoOsList *oslist = NULL;
    GList *osel = NULL, *tmp;
    gchar *debugstr;

    session = soup_session_async_new_with_options(
#ifdef HAVE_LIBSOUP_GNOME
                                                  SOUP_SESSION_ADD_FEATURE_BY_TYPE,
                                                  SOUP_TYPE_PROXY_RESOLVER_GNOME,
#endif
                                                  NULL);
    if ((debugstr = getenv("LIBOSINFO_TEST_DEBUG"))) {
        SoupLogger *logger;
        int debug_level = atoi(debugstr);

        logger = soup_logger_new (debug_level, -1);
        soup_session_add_feature (session, SOUP_SESSION_FEATURE (logger));
        g_object_unref (logger);
    }

    fail_unless(OSINFO_IS_LOADER(loader), "Loader is not a LOADER");
    fail_unless(OSINFO_IS_DB(db), "Db is not a DB");

    osinfo_loader_process_path(loader, SRCDIR "/data", &error);
    fail_unless(error == NULL, error ? error->message : "none");

    oslist = osinfo_db_get_os_list(db);
    tmp = osel = osinfo_list_get_elements(OSINFO_LIST(oslist));
    while (tmp) {
        OsinfoOs *os = tmp->data;
        OsinfoMediaList *medialist = osinfo_os_get_media_list(os);

        test_media(medialist, &error, session);

        fail_unless(error == NULL, error ? error->message : "none");

        g_object_unref(medialist);
        tmp = tmp->next;
    }

    g_list_free(tmp);
    if (oslist)
        g_object_unref(oslist);

    g_object_unref(loader);
}
END_TEST



static Suite *
list_suite(void)
{
    Suite *s = suite_create("List");
    TCase *tc = tcase_create("Core");
    tcase_set_timeout(tc, 120);

    tcase_add_test(tc, test_uris);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = list_suite ();
    SRunner *sr = srunner_create (s);

    if (!getenv("LIBOSINFO_NETWORK_TESTS"))
        return 77; /* Skip */

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
