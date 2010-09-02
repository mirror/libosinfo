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

    OsinfoDeviceLink *link1 = osinfo_os_add_device(os, NULL, dev1);
    osinfo_entity_add_param(OSINFO_ENTITY(link1), "device", "pci-e1000");
    OsinfoDeviceLink *link2 = osinfo_os_add_device(os, NULL, dev2);
    osinfo_entity_add_param(OSINFO_ENTITY(link2), "device", "pci-8139");

    OsinfoDeviceList *devices = osinfo_os_get_devices(os, NULL, NULL);

    fail_unless(osinfo_list_get_length(OSINFO_LIST(devices)) == 2, "Os has two devices");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(devices), 0) == OSINFO_ENTITY(dev1), "Got device 1");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(devices), 1) == OSINFO_ENTITY(dev2), "Got device 2");

    g_object_unref(devices);
    g_object_unref(dev1);
    g_object_unref(dev2);
    g_object_unref(os);
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

    OsinfoDeviceLink *link1 = osinfo_os_add_device(os, NULL, dev1);
    osinfo_entity_add_param(OSINFO_ENTITY(link1), "driver", "pci-e1000");
    OsinfoDeviceLink *link2 = osinfo_os_add_device(os, NULL, dev2);
    osinfo_entity_add_param(OSINFO_ENTITY(link2), "driver", "isa-sb16");

    osinfo_filter_add_constraint(filter, "class", "network");

    OsinfoDeviceList *devices = osinfo_os_get_devices(os, NULL, filter);

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


START_TEST(test_hv_devices)
{
    OsinfoOs *os = osinfo_os_new("awesome");
    OsinfoPlatform *hv = osinfo_platform_new("magical");
    OsinfoDevice *dev1 = osinfo_device_new("e1000");
    OsinfoDevice *dev2 = osinfo_device_new("rtl8139");
    OsinfoDevice *dev3 = osinfo_device_new("virtio");

    OsinfoDeviceLink *link1 = osinfo_os_add_device(os, NULL, dev1);
    osinfo_entity_add_param(OSINFO_ENTITY(link1), "device", "pci-e1000");
    OsinfoDeviceLink *link2 = osinfo_os_add_device(os, NULL, dev2);
    osinfo_entity_add_param(OSINFO_ENTITY(link2), "device", "pci-8139");

    OsinfoDeviceLink *link3 = osinfo_os_add_device(os, hv, dev1);
    osinfo_entity_add_param(OSINFO_ENTITY(link3), "device", "pci-e1000");
    OsinfoDeviceLink *link4 = osinfo_os_add_device(os, hv, dev3);
    osinfo_entity_add_param(OSINFO_ENTITY(link4), "device", "pci-virtio");

    OsinfoDeviceList *devices = osinfo_os_get_devices(os, NULL, NULL);
    fail_unless(osinfo_list_get_length(OSINFO_LIST(devices)) == 2, "Os has two devices");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(devices), 0) == OSINFO_ENTITY(dev1), "Got device 1");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(devices), 1) == OSINFO_ENTITY(dev2), "Got device 2");
    g_object_unref(devices);

    devices = osinfo_os_get_devices(os, hv, NULL);
    fail_unless(osinfo_list_get_length(OSINFO_LIST(devices)) == 2, "Os has two devices");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(devices), 0) == OSINFO_ENTITY(dev1), "Got device 1");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(devices), 1) == OSINFO_ENTITY(dev3), "Got device 3");
    g_object_unref(devices);

    g_object_unref(hv);
    g_object_unref(dev1);
    g_object_unref(dev2);
    g_object_unref(dev3);
    g_object_unref(os);
}
END_TEST


START_TEST(test_hv_devices_filter)
{
    OsinfoOs *os = osinfo_os_new("awesome");
    OsinfoPlatform *hv = osinfo_platform_new("magical");
    OsinfoDevice *dev1 = osinfo_device_new("e1000");
    OsinfoDevice *dev2 = osinfo_device_new("sb16");
    OsinfoDevice *dev3 = osinfo_device_new("virtio");
    OsinfoFilter *filter = osinfo_filter_new();

    osinfo_entity_add_param(OSINFO_ENTITY(dev1), "class", "network");
    osinfo_entity_add_param(OSINFO_ENTITY(dev2), "class", "audio");
    osinfo_entity_add_param(OSINFO_ENTITY(dev3), "class", "network");

    OsinfoDeviceLink *link1 = osinfo_os_add_device(os, NULL, dev1);
    osinfo_entity_add_param(OSINFO_ENTITY(link1), "device", "pci-e1000");
    OsinfoDeviceLink *link2 = osinfo_os_add_device(os, NULL, dev2);
    osinfo_entity_add_param(OSINFO_ENTITY(link2), "device", "isa-sb16");

    OsinfoDeviceLink *link3 = osinfo_os_add_device(os, hv, dev2);
    osinfo_entity_add_param(OSINFO_ENTITY(link3), "device", "pci-sb16");
    OsinfoDeviceLink *link4 = osinfo_os_add_device(os, hv, dev3);
    osinfo_entity_add_param(OSINFO_ENTITY(link4), "device", "pci-virtio");

    osinfo_filter_add_constraint(filter, "class", "network");

    OsinfoDeviceList *devices = osinfo_os_get_devices(os, NULL, filter);

    fail_unless(osinfo_list_get_length(OSINFO_LIST(devices)) == 1, "Os has one devices");
    OsinfoEntity *ent = osinfo_list_get_nth(OSINFO_LIST(devices), 0);
    fail_unless(OSINFO_IS_DEVICE(ent), "entity is a device");
    fail_unless(OSINFO_DEVICE(ent) == dev1, "device is e1000");

    g_object_unref(devices);

    devices = osinfo_os_get_devices(os, hv, filter);

    fail_unless(osinfo_list_get_length(OSINFO_LIST(devices)) == 1, "Os has one devices");
    ent = osinfo_list_get_nth(OSINFO_LIST(devices), 0);
    fail_unless(OSINFO_IS_DEVICE(ent), "entity is a device");
    fail_unless(OSINFO_DEVICE(ent) == dev3, "device is virtio");


    g_object_unref(devices);
    g_object_unref(filter);
    g_object_unref(dev1);
    g_object_unref(dev2);
    g_object_unref(dev3);
    g_object_unref(hv);
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

    OsinfoDeviceLink *link1 = osinfo_os_add_device(os, NULL, dev1);
    osinfo_entity_add_param(OSINFO_ENTITY(link1), "driver", "pci-e1000");
    OsinfoDeviceLink *link2 = osinfo_os_add_device(os, NULL, dev2);
    osinfo_entity_add_param(OSINFO_ENTITY(link2), "driver", "pci-8139");

    osinfo_filter_add_constraint(filter, "class", "network");

    const gchar *driver = NULL;
    OsinfoDevice *dev = osinfo_os_get_preferred_device(os, NULL, filter);
    OsinfoDeviceLink *link = osinfo_os_get_preferred_device_link(os, NULL, filter);
    fail_unless(OSINFO_IS_DEVICE(dev), "Got a preferred device");
    fail_unless(OSINFO_IS_DEVICELINK(link), "Got a preferred device link");
    fail_unless(dev == dev1, "Got preferred device e1000");
    fail_unless(link == link1, "Got preferred device link e1000");
    driver = osinfo_entity_get_param_value(OSINFO_ENTITY(link), "driver");
    fail_unless(dev == osinfo_devicelink_get_target(link), "Link target is not dev");
    fail_unless(g_strcmp0(driver, "pci-e1000") == 0, "Driver is pci-e1000");

    osinfo_filter_clear_constraints(filter);
    osinfo_filter_add_constraint(filter, "class", "audio");

    dev = osinfo_os_get_preferred_device(os, NULL, filter);
    link = osinfo_os_get_preferred_device_link(os, NULL, filter);
    fail_unless(dev == NULL, "Got unexpected device");
    fail_unless(link == NULL, "Got unexpected device link");

    g_object_unref(filter);
    g_object_unref(dev1);
    g_object_unref(dev2);
    g_object_unref(os);
}
END_TEST


START_TEST(test_hv_device_driver)
{
    OsinfoOs *os = osinfo_os_new("awesome");
    OsinfoPlatform *hv = osinfo_platform_new("special");
    OsinfoDevice *dev1 = osinfo_device_new("e1000");
    OsinfoDevice *dev2 = osinfo_device_new("rtl8139");
    OsinfoDevice *dev3 = osinfo_device_new("sb16");
    OsinfoFilter *filter = osinfo_filter_new();

    osinfo_entity_add_param(OSINFO_ENTITY(dev1), "class", "network");
    osinfo_entity_add_param(OSINFO_ENTITY(dev2), "class", "network");
    osinfo_entity_add_param(OSINFO_ENTITY(dev3), "class", "audio");

    OsinfoDeviceLink *link1 = osinfo_os_add_device(os, NULL, dev1);
    osinfo_entity_add_param(OSINFO_ENTITY(link1), "driver", "pci-e1000");
    OsinfoDeviceLink *link2 = osinfo_os_add_device(os, NULL, dev2);
    osinfo_entity_add_param(OSINFO_ENTITY(link2), "driver", "pci-8139");
    OsinfoDeviceLink *link3 = osinfo_os_add_device(os, NULL, dev3);
    osinfo_entity_add_param(OSINFO_ENTITY(link3), "driver", "pci-8139");
    OsinfoDeviceLink *link4 = osinfo_os_add_device(os, hv, dev1);
    osinfo_entity_add_param(OSINFO_ENTITY(link4), "driver", "pci-e1000");
    OsinfoDeviceLink *link5 = osinfo_os_add_device(os, hv, dev2);
    osinfo_entity_add_param(OSINFO_ENTITY(link5), "driver", "pci-8139");

    osinfo_filter_add_constraint(filter, "class", "network");

    const gchar *driver = NULL;
    OsinfoDevice *dev = osinfo_os_get_preferred_device(os, NULL, filter);
    OsinfoDeviceLink *link = osinfo_os_get_preferred_device_link(os, NULL, filter);
    fail_unless(OSINFO_IS_DEVICE(dev), "Got a preferred device");
    fail_unless(OSINFO_IS_DEVICELINK(link), "Got a preferred device link");
    fail_unless(dev == dev1, "Got preferred device e1000");
    fail_unless(link == link1, "Got preferred device link e1000");
    driver = osinfo_entity_get_param_value(OSINFO_ENTITY(link), "driver");
    fail_unless(dev == osinfo_devicelink_get_target(link), "Link target is not dev");
    fail_unless(g_strcmp0(driver, "pci-e1000") == 0, "Driver is pci-e1000");

    osinfo_filter_clear_constraints(filter);
    osinfo_filter_add_constraint(filter, "class", "audio");

    dev = osinfo_os_get_preferred_device(os, hv, filter);
    link = osinfo_os_get_preferred_device_link(os, hv, filter);
    fail_unless(dev == NULL, "Got unexpected device");
    fail_unless(link == NULL, "Got unexpected device link");

    g_object_unref(filter);
    g_object_unref(dev1);
    g_object_unref(dev2);
    g_object_unref(dev3);
    g_object_unref(hv);
    g_object_unref(os);
}
END_TEST


static Suite *
os_suite(void)
{
    Suite *s = suite_create("Os");
    TCase *tc = tcase_create("Core");
    tcase_add_test(tc, test_basic);
    tcase_add_test(tc, test_devices);
    tcase_add_test(tc, test_devices_filter);
    tcase_add_test(tc, test_device_driver);
    tcase_add_test(tc, test_hv_devices);
    tcase_add_test(tc, test_hv_devices_filter);
    tcase_add_test(tc, test_hv_device_driver);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = os_suite ();
    SRunner *sr = srunner_create (s);

    g_type_init();

    /* Upfront so we don't confuse valgrind */
    osinfo_platform_get_type();
    osinfo_device_get_type();
    osinfo_os_get_type();
    osinfo_oslist_get_type();
    osinfo_devicelist_get_type();
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
