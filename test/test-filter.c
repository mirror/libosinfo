#include <stdlib.h>
#include <osinfo/osinfo.h>
#include <check.h>



START_TEST(test_basic)
{
  OsinfoFilter *filter = osinfo_filter_new();
  OsinfoDevice *dev = osinfo_device_new("e1000");

  fail_unless(OSINFO_IS_FILTER(filter), "Filter is a filter object");
  fail_unless(osinfo_filter_matches(filter, OSINFO_ENTITY(dev)), "Filter matches device");

  osinfo_filter_add_constraint(filter, "class", "network");
  GList *keys = osinfo_filter_get_constraint_keys(filter);
  fail_unless(keys != NULL, "missing key");
  fail_unless(g_strcmp0(keys->data, "class") == 0, "missing key");
  fail_unless(keys->next == NULL, "too many keys");
  g_list_free(keys);

  osinfo_filter_add_constraint(filter, "class", "audio");
  keys = osinfo_filter_get_constraint_keys(filter);
  fail_unless(keys != NULL, "missing key");
  fail_unless(g_strcmp0(keys->data, "class") == 0, "missing key");
  fail_unless(keys->next == NULL, "too many keys");
  g_list_free(keys);

  osinfo_filter_add_constraint(filter, "bus", "pci");
  keys = osinfo_filter_get_constraint_keys(filter);
  fail_unless(keys != NULL, "missing key");
  fail_unless(keys->next != NULL, "not enough keys");
  fail_unless(g_strcmp0(keys->data, "bus") == 0, "missing bus key");
  fail_unless(g_strcmp0(keys->next->data, "class") == 0, "missing class key");
  fail_unless(keys->next->next == NULL, "too many keys");
  g_list_free(keys);

  GList *values = osinfo_filter_get_constraint_values(filter, "bus");
  fail_unless(values != NULL, "missing value");
  fail_unless(g_strcmp0(values->data, "pci") == 0, "missing value");
  fail_unless(values->next == NULL, "too many keys");

  values = osinfo_filter_get_constraint_values(filter, "class");
  fail_unless(values != NULL, "missing value");
  fail_unless(values->next != NULL, "not enough values");
  fail_unless(g_strcmp0(values->data, "audio") == 0, "missing value");
  fail_unless(g_strcmp0(values->next->data, "network") == 0, "missing value");
  fail_unless(values->next->next == NULL, "too many values");

  g_object_unref(dev);
  g_object_unref(filter);
}
END_TEST

START_TEST(test_filter_single)
{
  OsinfoFilter *filter = osinfo_filter_new();
  OsinfoDevice *dev = osinfo_device_new("e1000");

  osinfo_entity_add_param(OSINFO_ENTITY(dev), "bus", "pci");

  osinfo_filter_add_constraint(filter, "class", "network");
  fail_unless(!osinfo_filter_matches(filter, OSINFO_ENTITY(dev)), "Filter does not match device");

  osinfo_entity_add_param(OSINFO_ENTITY(dev), "class", "network");
  fail_unless(osinfo_filter_matches(filter, OSINFO_ENTITY(dev)), "Filter matches device");

  osinfo_filter_clear_constraint(filter, "class");
  osinfo_filter_add_constraint(filter, "class", "audio");
  fail_unless(!osinfo_filter_matches(filter, OSINFO_ENTITY(dev)), "Filter does not match device");

  g_object_unref(dev);
  g_object_unref(filter);
}
END_TEST

START_TEST(test_filter_multi)
{
  OsinfoFilter *filter = osinfo_filter_new();
  OsinfoDevice *dev = osinfo_device_new("e1000");

  osinfo_entity_add_param(OSINFO_ENTITY(dev), "bus", "pci");

  osinfo_filter_add_constraint(filter, "bus", "isa");
  fail_unless(!osinfo_filter_matches(filter, OSINFO_ENTITY(dev)), "Filter does not match device");

  osinfo_filter_add_constraint(filter, "bus", "pci");
  /* XXX is this right ?  Multiple values for a filter constraint 
   * is treated as requiring all constraint values to match, not
   * required any to match */
  //fail_unless(osinfo_filter_matches(filter, OSINFO_ENTITY(dev)), "Filter matches device");
  fail_unless(!osinfo_filter_matches(filter, OSINFO_ENTITY(dev)), "Filter does not match device");

  osinfo_filter_clear_constraints(filter);
  osinfo_filter_add_constraint(filter, "bus", "pci");
  fail_unless(osinfo_filter_matches(filter, OSINFO_ENTITY(dev)), "Filter matches device");

  g_object_unref(dev);
  g_object_unref(filter);
}
END_TEST


START_TEST(test_filter_combine)
{
  OsinfoFilter *filter = osinfo_filter_new();
  OsinfoDevice *dev1 = osinfo_device_new("e1000");
  OsinfoDevice *dev2 = osinfo_device_new("ne2k");

  osinfo_entity_add_param(OSINFO_ENTITY(dev1), "bus", "pci");
  osinfo_entity_add_param(OSINFO_ENTITY(dev1), "class", "network");

  osinfo_entity_add_param(OSINFO_ENTITY(dev2), "bus", "isa");
  osinfo_entity_add_param(OSINFO_ENTITY(dev2), "class", "network");

  osinfo_filter_add_constraint(filter, "class", "network");
  fail_unless(osinfo_filter_matches(filter, OSINFO_ENTITY(dev1)), "Filter does not match device");
  fail_unless(osinfo_filter_matches(filter, OSINFO_ENTITY(dev2)), "Filter does not match device");

  osinfo_filter_add_constraint(filter, "bus", "isa");
  fail_unless(!osinfo_filter_matches(filter, OSINFO_ENTITY(dev1)), "Filter match device");
  fail_unless(osinfo_filter_matches(filter, OSINFO_ENTITY(dev2)), "Filter does not match device");

  g_object_unref(dev1);
  g_object_unref(dev2);
  g_object_unref(filter);
}
END_TEST


static Suite *
filter_suite(void)
{
  Suite *s = suite_create("Filter");
  TCase *tc = tcase_create("Core");
  tcase_add_test(tc, test_basic);
  tcase_add_test(tc, test_filter_single);
  tcase_add_test(tc, test_filter_multi);
  tcase_add_test(tc, test_filter_combine);
  suite_add_tcase(s, tc);
  return s;
}

int main(void)
{
  int number_failed;
  Suite *s = filter_suite ();
  SRunner *sr = srunner_create (s);

  g_type_init();

  /* Upfront so we don't confuse valgrind */
  osinfo_device_get_type();
  osinfo_filter_get_type();

  srunner_run_all (sr, CK_ENV);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
