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



START_TEST(test_basic)
{
    OsinfoOs *os = osinfo_os_new("pony");

    fail_unless(OSINFO_IS_OS(os), "Os is a os object");
    fail_unless(g_strcmp0(osinfo_entity_get_id(OSINFO_ENTITY(os)), "pony") == 0, "Os ID was pony");

    g_object_unref(os);
}
END_TEST

START_TEST(test_devices)
{
    OsinfoOs *os = osinfo_os_new("awesome");
    OsinfoDevice *dev1 = osinfo_device_new("e1000");
    OsinfoDevice *dev2 = osinfo_device_new("rtl8139");

    OsinfoDeviceLink *link1 = osinfo_os_add_device(os, dev1);
    osinfo_entity_add_param(OSINFO_ENTITY(link1), "device", "pci-e1000");
    OsinfoDeviceLink *link2 = osinfo_os_add_device(os, dev2);
    osinfo_entity_add_param(OSINFO_ENTITY(link2), "device", "pci-8139");

    OsinfoDeviceList *devices = osinfo_os_get_devices(os, NULL);

    fail_unless(osinfo_list_get_length(OSINFO_LIST(devices)) == 2, "Os has two devices");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(devices), 0) == OSINFO_ENTITY(dev1), "Got device 1");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(devices), 1) == OSINFO_ENTITY(dev2), "Got device 2");

    g_object_unref(devices);
    g_object_unref(dev1);
    g_object_unref(dev2);
    g_object_unref(os);
}
END_TEST


START_TEST(test_loader)
{
    OsinfoLoader *loader;
    OsinfoDb *db;
    OsinfoOs *os;
    GError *error = NULL;
    const char *str;

    loader = osinfo_loader_new();
    osinfo_loader_process_path(loader, SRCDIR "/test/dbdata", &error);
    fail_unless(error == NULL, error ? error->message:"none");
    db = osinfo_loader_get_db(loader);

    os = osinfo_db_get_os(db, "http://libosinfo.org/test/os/test1");
    fail_unless(os != NULL, "could not find OS 'test1'");
    str = osinfo_product_get_short_id(OSINFO_PRODUCT(os));
    fail_unless(g_strcmp0(str, "test1") == 0, "wrong OS short-id");
    str = osinfo_product_get_name(OSINFO_PRODUCT(os));
    fail_unless(g_strcmp0(str, "Test 1") == 0, "wrong OS name");
    str = osinfo_product_get_version(OSINFO_PRODUCT(os));
    fail_unless(g_strcmp0(str, "unknown") == 0, "wrong OS version");
    str = osinfo_product_get_vendor(OSINFO_PRODUCT(os));
    fail_unless(g_strcmp0(str, "libosinfo.org") == 0, "wrong OS vendor");
    str = osinfo_os_get_family(os);
    fail_unless(g_strcmp0(str, "test") == 0, "wrong OS family");
    fail_unless(osinfo_os_get_release_status(os) == OSINFO_RELEASE_STATUS_PRERELEASE,
                "OS should be a pre-release");

    os = osinfo_db_get_os(db, "http://libosinfo.org/test/os/test2");
    fail_unless(os != NULL, "could not find OS 'test2'");
    str = osinfo_product_get_short_id(OSINFO_PRODUCT(os));
    fail_unless(g_strcmp0(str, "test2") == 0, "wrong OS short-id");
    str = osinfo_product_get_name(OSINFO_PRODUCT(os));
    fail_unless(str == NULL, "wrong OS name");
    str = osinfo_product_get_version(OSINFO_PRODUCT(os));
    fail_unless(str == NULL, "wrong OS version");
    str = osinfo_product_get_vendor(OSINFO_PRODUCT(os));
    fail_unless(str == NULL, "wrong OS vendor");
    str = osinfo_os_get_family(os);
    fail_unless(str == NULL, "wrong OS family");
    fail_unless(osinfo_os_get_release_status(os) == OSINFO_RELEASE_STATUS_RELEASED,
                "OS should be a released one");

    os = osinfo_db_get_os(db, "http://libosinfo.org/test/os/test3");
    fail_unless(os != NULL, "could not find OS 'test3'");
    str = osinfo_product_get_short_id(OSINFO_PRODUCT(os));
    fail_unless(g_strcmp0(str, "test3") == 0, "wrong OS short-id");
    fail_unless(osinfo_os_get_release_status(os) == OSINFO_RELEASE_STATUS_RELEASED,
                "OS should be a released one");

    os = osinfo_db_get_os(db, "http://libosinfo.org/test/os/test4");
    fail_unless(os != NULL, "could not find OS 'test4'");
    str = osinfo_product_get_short_id(OSINFO_PRODUCT(os));
    fail_unless(g_strcmp0(str, "test4") == 0, "wrong OS short-id");
    fail_unless(osinfo_os_get_release_status(os) == OSINFO_RELEASE_STATUS_SNAPSHOT,
                "OS should be a snapshot");

    os = osinfo_db_get_os(db, "http://libosinfo.org/test/os/test5");
    fail_unless(os != NULL, "could not find OS 'test5'");
    str = osinfo_product_get_short_id(OSINFO_PRODUCT(os));
    fail_unless(g_strcmp0(str, "test5") == 0, "wrong OS short-id");
    fail_unless(osinfo_os_get_release_status(os) == OSINFO_RELEASE_STATUS_RELEASED,
                "OS should be a released one");

    g_object_unref(loader);
}
END_TEST


START_TEST(test_devices_filter)
{
    OsinfoOs *os = osinfo_os_new("awesome");
    OsinfoDevice *dev1 = osinfo_device_new("e1000");
    OsinfoDevice *dev2 = osinfo_device_new("sb16");
    OsinfoFilter *filter = osinfo_filter_new();

    osinfo_entity_add_param(OSINFO_ENTITY(dev1), "class", "network");
    osinfo_entity_add_param(OSINFO_ENTITY(dev2), "class", "audio");

    OsinfoDeviceLink *link1 = osinfo_os_add_device(os, dev1);
    osinfo_entity_add_param(OSINFO_ENTITY(link1), "driver", "pci-e1000");
    OsinfoDeviceLink *link2 = osinfo_os_add_device(os, dev2);
    osinfo_entity_add_param(OSINFO_ENTITY(link2), "driver", "isa-sb16");

    osinfo_filter_add_constraint(filter, "class", "network");

    OsinfoDeviceList *devices = osinfo_os_get_devices(os, filter);

    fail_unless(osinfo_list_get_length(OSINFO_LIST(devices)) == 1, "Os has one devices");
    OsinfoEntity *ent = osinfo_list_get_nth(OSINFO_LIST(devices), 0);
    fail_unless(OSINFO_IS_DEVICE(ent), "entity is a device");
    fail_unless(OSINFO_DEVICE(ent) == dev1, "device is e1000");

    g_object_unref(devices);
    g_object_unref(filter);
    g_object_unref(dev1);
    g_object_unref(dev2);
    g_object_unref(os);
}
END_TEST


START_TEST(test_device_driver)
{
    OsinfoOs *os = osinfo_os_new("awesome");
    OsinfoDevice *dev1 = osinfo_device_new("e1000");
    OsinfoDevice *dev2 = osinfo_device_new("rtl8139");
    OsinfoFilter *filter = osinfo_filter_new();

    osinfo_entity_add_param(OSINFO_ENTITY(dev1), "class", "network");
    osinfo_entity_add_param(OSINFO_ENTITY(dev2), "class", "network");

    OsinfoDeviceLink *link1 = osinfo_os_add_device(os, dev1);
    osinfo_entity_add_param(OSINFO_ENTITY(link1), "driver", "pci-e1000");
    OsinfoDeviceLink *link2 = osinfo_os_add_device(os, dev2);
    osinfo_entity_add_param(OSINFO_ENTITY(link2), "driver", "pci-8139");

    osinfo_filter_add_constraint(filter, "class", "network");

    osinfo_filter_clear_constraints(filter);
    osinfo_filter_add_constraint(filter, "class", "audio");

    g_object_unref(filter);
    g_object_unref(dev1);
    g_object_unref(dev2);
    g_object_unref(os);
}
END_TEST


static Suite *
os_suite(void)
{
    Suite *s = suite_create("Os");
    TCase *tc = tcase_create("Core");
    tcase_add_test(tc, test_basic);
    tcase_add_test(tc, test_loader);
    tcase_add_test(tc, test_devices);
    tcase_add_test(tc, test_devices_filter);
    tcase_add_test(tc, test_device_driver);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = os_suite();
    SRunner *sr = srunner_create(s);

    /* Upfront so we don't confuse valgrind */
    osinfo_platform_get_type();
    osinfo_device_get_type();
    osinfo_os_get_type();
    osinfo_oslist_get_type();
    osinfo_devicelist_get_type();
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
