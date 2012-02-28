/*
 * Copyright (C) 2009-2012 Red Hat, Inc.
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

#include <config.h>
#include <stdlib.h>
#include <osinfo/osinfo.h>
#include <check.h>

static GError *error = NULL;
static gchar *actualData = NULL;
static const gchar *expectData =                                        \
    "\n"                                                                \
    "# OS id=http://fedoraproject.org/fedora/16 profile jeos\n"         \
    "install\n"                                                         \
    "text\n"                                                            \
    "keyboard uk\n"                                                     \
    "lang en_GB.UTF-8\n"                                                \
    "skipx\n"                                                           \
    "network --device eth0 --bootproto dhcp\n"                          \
    "rootpw 123456\n"                                                   \
    "timezone --utc Europe/London\n"                                    \
    "bootloader --location=mbr\n"                                       \
    "zerombr\n"                                                         \
    "\n"                                                                \
    "part biosboot --fstype=biosboot --size=1\n"                        \
    "part pv.2 --size=1 --grow --ondisk=vda\n"                          \
    "volgroup VolGroup00 --pesize=32768 pv.2\n"                         \
    "logvol / --fstype ext4 --name=LogVol00 --vgname=VolGroup00 --size=1024 --grow\n" \
    "reboot\n"                                                          \
    "\n"                                                                \
    "%packages\n"                                                       \
    "@base\n"                                                           \
    "@core\n"                                                           \
    "@hardware-support\n"                                               \
    "\n"                                                                \
    "%end\n"                                                            \
    "  ";

static void test_generate_finish(GObject *src,
                                 GAsyncResult *res,
                                 gpointer user_data)
{
    GMainLoop *loop = user_data;

    actualData = osinfo_install_script_generate_finish(OSINFO_INSTALL_SCRIPT(src),
                                                       res,
                                                       &error);

    g_main_loop_quit(loop);
}

static OsinfoInstallConfig *test_get_config(void)
{
    OsinfoInstallConfig *config;

    config = osinfo_install_config_new("http://example.com");

    osinfo_install_config_set_l10n_keyboard(config, "uk");
    osinfo_install_config_set_l10n_language(config, "en_GB.UTF-8");
    osinfo_install_config_set_l10n_timezone(config, "Europe/London");

    osinfo_install_config_set_admin_password(config, "123456");

    osinfo_install_config_set_user_password(config, "123456");
    osinfo_install_config_set_user_login(config, "fred");
    osinfo_install_config_set_user_realname(config, "Fred Blogs");
    osinfo_install_config_set_user_autologin(config, TRUE);
    osinfo_install_config_set_user_administrator(config, TRUE);

    return config;
}

START_TEST(test_script_file)
{
    OsinfoInstallScript *script;
    OsinfoInstallConfig *config = test_get_config();
    OsinfoOs *os;
    GMainLoop *loop;

    script = osinfo_install_script_new_uri("http://example.com",
                                           "jeos",
                                           "file://" SRCDIR "/test/install-script.xsl");

    loop = g_main_loop_new (g_main_context_get_thread_default (),
                            TRUE);

    os = osinfo_os_new("http://fedoraproject.org/fedora/16");
    osinfo_install_script_generate_async(script,
                                         os,
                                         config,
                                         NULL,
                                         test_generate_finish,
                                         loop);

    if (g_main_loop_is_running(loop))
        g_main_loop_run(loop);

    unlink(BUILDDIR "/test/install-script-actual.txt");
    fail_unless(error == NULL, error ? error->message : "none");

    fail_unless(strcmp(actualData, expectData) == 0, "Actual '%s' match expect '%s'",
                actualData, expectData);

    g_object_unref(os);
    g_object_unref(config);
    g_object_unref(script);
    g_main_loop_unref(loop);
}
END_TEST



START_TEST(test_script_data)
{
    OsinfoInstallScript *script;
    OsinfoInstallConfig *config = test_get_config();
    OsinfoOs *os;
    GMainLoop *loop;
    GFile *file = g_file_new_for_uri("file://" SRCDIR "/test/install-script.xsl");
    gchar *data;

    g_file_load_contents(file, NULL, &data, NULL, NULL, &error);
    fail_unless(error == NULL, error ? error->message : "none");

    os = osinfo_os_new("http://fedoraproject.org/fedora/16");
    osinfo_entity_set_param(OSINFO_ENTITY(os),
                            OSINFO_PRODUCT_PROP_SHORT_ID,
                            "fedora16");
    script = osinfo_install_script_new_data("http://example.com",
                                            "jeos",
                                            data);

    loop = g_main_loop_new (g_main_context_get_thread_default (),
                            TRUE);

    osinfo_install_script_generate_async(script,
                                         os,
                                         config,
                                         NULL,
                                         test_generate_finish,
                                         loop);

    if (g_main_loop_is_running(loop))
        g_main_loop_run(loop);

    unlink(BUILDDIR "/test/install-script-actual.txt");
    fail_unless(error == NULL, error ? error->message : "none");

    g_object_unref(os);
    g_object_unref(config);
    g_object_unref(script);
}
END_TEST



static Suite *
list_suite(void)
{
    Suite *s = suite_create("List");
    TCase *tc = tcase_create("Core");
    tcase_set_timeout(tc, 120);

    tcase_add_test(tc, test_script_file);
    tcase_add_test(tc, test_script_data);
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
