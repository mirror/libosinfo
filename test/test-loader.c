#include <stdlib.h>
#include <osinfo/osinfo.h>
#include <check.h>

START_TEST(test_basic)
{
  OsinfoLoader *loader = osinfo_loader_new();

  fail_unless(OSINFO_IS_LOADER(loader), "Loader is not a LOADER");

  GError *error = NULL;
  osinfo_loader_process(loader, "../data", &error);
  fail_unless(error == NULL, error ? error->message : "none");

  g_object_unref(loader);
}
END_TEST

static Suite *
loader_suite(void)
{
  Suite *s = suite_create("Loader");
  TCase *tc = tcase_create("Core");
  tcase_add_test(tc, test_basic);
  suite_add_tcase(s, tc);
  return s;
}

int main(void)
{
  int number_failed;
  Suite *s = loader_suite ();
  SRunner *sr = srunner_create (s);

  g_type_init();

  /* Upfront so we don't confuse valgrind */
  osinfo_entity_get_type();
  osinfo_db_get_type();
  osinfo_device_get_type();
  osinfo_hypervisor_get_type();
  osinfo_os_get_type();
  osinfo_list_get_type();
  osinfo_devicelist_get_type();
  osinfo_hypervisorlist_get_type();
  osinfo_oslist_get_type();
  osinfo_filter_get_type();
  osinfo_loader_get_type();

  srunner_run_all (sr, CK_ENV);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
