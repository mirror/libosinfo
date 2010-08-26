#include <stdlib.h>
#include <osinfo/osinfo.h>
#include <check.h>



START_TEST(test_basic)
{
  OsinfoDevice *device = osinfo_device_new("e1000");

  fail_unless(OSINFO_IS_DEVICE(device), "Device is a device object");
  fail_unless(g_strcmp0(osinfo_entity_get_id(OSINFO_ENTITY(device)), "e1000") == 0, "Device ID was e1000");

  g_object_unref(device);
}
END_TEST


static Suite *
device_suite(void)
{
  Suite *s = suite_create("Device");
  TCase *tc = tcase_create("Core");
  tcase_add_test(tc, test_basic);
  suite_add_tcase(s, tc);
  return s;
}

int main(void)
{
  int number_failed;
  Suite *s = device_suite ();
  SRunner *sr = srunner_create (s);

  g_type_init();

  /* Upfront so we don't confuse valgrind */
  osinfo_device_get_type();

  srunner_run_all (sr, CK_ENV);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
