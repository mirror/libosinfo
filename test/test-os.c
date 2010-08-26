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

  osinfo_os_add_device(os, NULL, dev1, "pci-e1000");
  osinfo_os_add_device(os, NULL, dev2, "pci-8139");

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

  osinfo_os_add_device(os, NULL, dev1, "pci-e1000");
  osinfo_os_add_device(os, NULL, dev2, "isa-sb16");

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
  OsinfoHypervisor *hv = osinfo_hypervisor_new("magical");
  OsinfoDevice *dev1 = osinfo_device_new("e1000");
  OsinfoDevice *dev2 = osinfo_device_new("rtl8139");
  OsinfoDevice *dev3 = osinfo_device_new("virtio");

  osinfo_os_add_device(os, NULL, dev1, "pci-e1000");
  osinfo_os_add_device(os, NULL, dev2, "pci-8139");

  osinfo_os_add_device(os, hv, dev1, "pci-e1000");
  osinfo_os_add_device(os, hv, dev3, "pci-virtio");

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
  OsinfoHypervisor *hv = osinfo_hypervisor_new("magical");
  OsinfoDevice *dev1 = osinfo_device_new("e1000");
  OsinfoDevice *dev2 = osinfo_device_new("sb16");
  OsinfoDevice *dev3 = osinfo_device_new("virtio");
  OsinfoFilter *filter = osinfo_filter_new();

  osinfo_entity_add_param(OSINFO_ENTITY(dev1), "class", "network");
  osinfo_entity_add_param(OSINFO_ENTITY(dev2), "class", "audio");
  osinfo_entity_add_param(OSINFO_ENTITY(dev3), "class", "network");

  osinfo_os_add_device(os, NULL, dev1, "pci-e1000");
  osinfo_os_add_device(os, NULL, dev2, "isa-sb16");

  osinfo_os_add_device(os, hv, dev2, "pci-sb16");
  osinfo_os_add_device(os, hv, dev3, "pci-virtio");

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


START_TEST(test_relos)
{
  OsinfoOs *os1 = osinfo_os_new("pony");
  OsinfoOs *os2 = osinfo_os_new("donkey");
  OsinfoOs *os3 = osinfo_os_new("wathog");
  OsinfoOs *os4 = osinfo_os_new("aardvark");
  OsinfoOs *os5 = osinfo_os_new("unicorn");

  osinfo_os_add_related_os(os1, OSINFO_OS_RELATIONSHIP_DERIVES_FROM, os2);
  osinfo_os_add_related_os(os1, OSINFO_OS_RELATIONSHIP_UPGRADES, os3);
  osinfo_os_add_related_os(os1, OSINFO_OS_RELATIONSHIP_UPGRADES, os4);
  osinfo_os_add_related_os(os3, OSINFO_OS_RELATIONSHIP_UPGRADES, os4);
  osinfo_os_add_related_os(os1, OSINFO_OS_RELATIONSHIP_CLONES, os5);

  OsinfoOsList *os1rel = osinfo_os_get_related(os1, OSINFO_OS_RELATIONSHIP_DERIVES_FROM);
  fail_unless(osinfo_list_get_length(OSINFO_LIST(os1rel)) == 1, "Os has 1 derived os");
  fail_unless(osinfo_list_get_nth(OSINFO_LIST(os1rel), 0) == OSINFO_ENTITY(os2), "derived os is os2");
  g_object_unref(os1rel);

  os1rel = osinfo_os_get_related(os1, OSINFO_OS_RELATIONSHIP_UPGRADES);
  fail_unless(osinfo_list_get_length(OSINFO_LIST(os1rel)) == 2, "Os has 2 upgraded os");
  fail_unless((osinfo_list_get_nth(OSINFO_LIST(os1rel), 0) == OSINFO_ENTITY(os3) ||
	       osinfo_list_get_nth(OSINFO_LIST(os1rel), 0) == OSINFO_ENTITY(os4)) &&
	      (osinfo_list_get_nth(OSINFO_LIST(os1rel), 1) == OSINFO_ENTITY(os3) ||
	       osinfo_list_get_nth(OSINFO_LIST(os1rel), 1) == OSINFO_ENTITY(os4)),
	      "upgraded oses are os3 + os4");
  g_object_unref(os1rel);

  os1rel = osinfo_os_get_related(os3, OSINFO_OS_RELATIONSHIP_UPGRADES);
  fail_unless(osinfo_list_get_length(OSINFO_LIST(os1rel)) == 1, "Os has 1 upgraded os");
  fail_unless(osinfo_list_get_nth(OSINFO_LIST(os1rel), 0) == OSINFO_ENTITY(os4), "upgraded os is os4");
  g_object_unref(os1rel);

  os1rel = osinfo_os_get_related(os1, OSINFO_OS_RELATIONSHIP_CLONES);
  fail_unless(osinfo_list_get_length(OSINFO_LIST(os1rel)) == 1, "Os has 1 upgraded os");
  fail_unless(osinfo_list_get_nth(OSINFO_LIST(os1rel), 0) == OSINFO_ENTITY(os5), "cloned os is os5");
  g_object_unref(os1rel);

  g_object_unref(os1);
  g_object_unref(os2);
  g_object_unref(os3);
  g_object_unref(os4);
  g_object_unref(os5);
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

  osinfo_os_add_device(os, NULL, dev1, "pci-e1000");
  osinfo_os_add_device(os, NULL, dev2, "pci-8139");

  osinfo_filter_add_constraint(filter, "class", "network");

  const gchar *driver = NULL;
  OsinfoDevice *dev = osinfo_os_get_preferred_device(os, NULL, filter, &driver);
  fail_unless(OSINFO_IS_DEVICE(dev), "Got a preferred device");
  fail_unless(dev == dev1, "Got preferred device e1000");
  fail_unless(g_strcmp0(driver, "pci-e1000") == 0, "Driver is pci-e1000");

  osinfo_filter_clear_constraints(filter);
  osinfo_filter_add_constraint(filter, "class", "audio");

  dev = osinfo_os_get_preferred_device(os, NULL, filter, &driver);
  fail_unless(dev == NULL, "Got unexpected device");
  fail_unless(driver == NULL, "Got unexpected driver");

  g_object_unref(filter);
  g_object_unref(dev1);
  g_object_unref(dev2);
  g_object_unref(os);
}
END_TEST


START_TEST(test_hv_device_driver)
{
  OsinfoOs *os = osinfo_os_new("awesome");
  OsinfoHypervisor *hv = osinfo_hypervisor_new("special");
  OsinfoDevice *dev1 = osinfo_device_new("e1000");
  OsinfoDevice *dev2 = osinfo_device_new("rtl8139");
  OsinfoDevice *dev3 = osinfo_device_new("sb16");
  OsinfoFilter *filter = osinfo_filter_new();

  osinfo_entity_add_param(OSINFO_ENTITY(dev1), "class", "network");
  osinfo_entity_add_param(OSINFO_ENTITY(dev2), "class", "network");
  osinfo_entity_add_param(OSINFO_ENTITY(dev3), "class", "audio");

  osinfo_os_add_device(os, NULL, dev1, "pci-e1000");
  osinfo_os_add_device(os, NULL, dev2, "pci-8139");
  osinfo_os_add_device(os, NULL, dev3, "pci-8139");
  osinfo_os_add_device(os, hv, dev1, "pci-e1000");
  osinfo_os_add_device(os, hv, dev2, "pci-8139");

  osinfo_filter_add_constraint(filter, "class", "network");

  const gchar *driver = NULL;
  OsinfoDevice *dev = osinfo_os_get_preferred_device(os, NULL, filter, &driver);
  fail_unless(OSINFO_IS_DEVICE(dev), "Got a preferred device");
  fail_unless(dev == dev1, "Got preferred device e1000");
  fail_unless(g_strcmp0(driver, "pci-e1000") == 0, "Driver is pci-e1000");

  osinfo_filter_clear_constraints(filter);
  osinfo_filter_add_constraint(filter, "class", "audio");

  dev = osinfo_os_get_preferred_device(os, hv, filter, &driver);
  fail_unless(dev == NULL, "Got unexpected device");
  fail_unless(driver == NULL, "Got unexpected driver");

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
  tcase_add_test(tc, test_relos);
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
  osinfo_hypervisor_get_type();
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
