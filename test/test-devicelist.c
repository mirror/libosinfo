#include <stdlib.h>
#include <osinfo/osinfo.h>
#include <check.h>


START_TEST(test_union)
{
  OsinfoDeviceList *list1 = osinfo_devicelist_new();
  OsinfoDeviceList *list2 = osinfo_devicelist_new();
  OsinfoDeviceList *list3;
  OsinfoDevice *ent1 = osinfo_device_new("wibble1");
  OsinfoDevice *ent2 = osinfo_device_new("wibble2");
  OsinfoDevice *ent3 = osinfo_device_new("wibble3");
  OsinfoDevice *ent4 = osinfo_device_new("wibble4");

  osinfo_list_add(OSINFO_LIST(list1), OSINFO_ENTITY(ent1));
  osinfo_list_add(OSINFO_LIST(list1), OSINFO_ENTITY(ent2));
  osinfo_list_add(OSINFO_LIST(list1), OSINFO_ENTITY(ent3));

  osinfo_list_add(OSINFO_LIST(list2), OSINFO_ENTITY(ent1));
  osinfo_list_add(OSINFO_LIST(list2), OSINFO_ENTITY(ent4));

  list3 = osinfo_devicelist_new_union(list1, list2);

  fail_unless(osinfo_list_get_length(OSINFO_LIST(list3)) == 4, "List did not have 4 elements");

  gboolean has1 = FALSE;
  gboolean has2 = FALSE;
  gboolean has3 = FALSE;
  gboolean has4 = FALSE;
  gboolean hasBad = FALSE;
  int i;
  for (i = 0 ; i < osinfo_list_get_length(OSINFO_LIST(list3)) ; i++) {
    OsinfoDevice *ent = OSINFO_DEVICE(osinfo_list_get_nth(OSINFO_LIST(list3), i));
    if (ent == ent1)
      has1 = TRUE;
    else if (ent == ent2)
      has2 = TRUE;
    else if (ent == ent3)
      has3 = TRUE;
    else if (ent == ent4)
      has4 = TRUE;
    else
      hasBad = TRUE;
  }
  fail_unless(has1, "List was missing entity 1");
  fail_unless(has2, "List was missing entity 2");
  fail_unless(has3, "List was missing entity 3");
  fail_unless(has4, "List was missing entity 4");
  fail_unless(!hasBad, "List had unexpected entity");

  g_object_unref(ent1);
  g_object_unref(ent2);
  g_object_unref(ent3);
  g_object_unref(ent4);
  g_object_unref(list1);
  g_object_unref(list2);
  g_object_unref(list3);
}
END_TEST


START_TEST(test_intersect)
{
  OsinfoDeviceList *list1 = osinfo_devicelist_new();
  OsinfoDeviceList *list2 = osinfo_devicelist_new();
  OsinfoDeviceList *list3;
  OsinfoDevice *ent1 = osinfo_device_new("wibble1");
  OsinfoDevice *ent2 = osinfo_device_new("wibble2");
  OsinfoDevice *ent3 = osinfo_device_new("wibble3");
  OsinfoDevice *ent4 = osinfo_device_new("wibble4");


  osinfo_list_add(OSINFO_LIST(list1), OSINFO_ENTITY(ent1));
  osinfo_list_add(OSINFO_LIST(list1), OSINFO_ENTITY(ent2));
  osinfo_list_add(OSINFO_LIST(list1), OSINFO_ENTITY(ent3));

  osinfo_list_add(OSINFO_LIST(list2), OSINFO_ENTITY(ent1));
  osinfo_list_add(OSINFO_LIST(list2), OSINFO_ENTITY(ent3));
  osinfo_list_add(OSINFO_LIST(list2), OSINFO_ENTITY(ent4));

  list3 = osinfo_devicelist_new_intersection(list1, list2);

  fail_unless(osinfo_list_get_length(OSINFO_LIST(list3)) == 2, "List did not have 2 elements");

  gboolean has1 = FALSE;
  gboolean has2 = FALSE;
  gboolean has3 = FALSE;
  gboolean has4 = FALSE;
  gboolean hasBad = FALSE;
  int i;
  for (i = 0 ; i < osinfo_list_get_length(OSINFO_LIST(list3)) ; i++) {
    OsinfoDevice *ent = OSINFO_DEVICE(osinfo_list_get_nth(OSINFO_LIST(list3), i));
    if (ent == ent1)
      has1 = TRUE;
    else if (ent == ent2)
      has2 = TRUE;
    else if (ent == ent3)
      has3 = TRUE;
    else if (ent == ent4)
      has4 = TRUE;
    else
      hasBad = TRUE;
  }
  fail_unless(has1, "List was missing entity 1");
  fail_unless(!has2, "List had unexpected entity 2");
  fail_unless(has3, "List was missing entity 3");
  fail_unless(!has4, "List had unexpected entity 4");
  fail_unless(!hasBad, "List had unexpected entity");

  g_object_unref(ent1);
  g_object_unref(ent2);
  g_object_unref(ent3);
  g_object_unref(ent4);
  g_object_unref(list1);
  g_object_unref(list2);
  g_object_unref(list3);
}
END_TEST


START_TEST(test_filter)
{
  OsinfoDeviceList *list1 = osinfo_devicelist_new();
  OsinfoDeviceList *list2;
  OsinfoFilter *filter = osinfo_filter_new();
  OsinfoDevice *ent1 = osinfo_device_new("wibble1");
  OsinfoDevice *ent2 = osinfo_device_new("wibble2");
  OsinfoDevice *ent3 = osinfo_device_new("wibble3");
  OsinfoDevice *ent4 = osinfo_device_new("wibble4");

  osinfo_entity_add_param(OSINFO_ENTITY(ent1), "class", "network");
  osinfo_entity_add_param(OSINFO_ENTITY(ent1), "class", "wilma");
  osinfo_entity_add_param(OSINFO_ENTITY(ent2), "class", "network");
  osinfo_entity_add_param(OSINFO_ENTITY(ent3), "class", "network");
  osinfo_entity_add_param(OSINFO_ENTITY(ent3), "class", "audio");
  osinfo_entity_add_param(OSINFO_ENTITY(ent4), "class", "audio");

  osinfo_filter_add_constraint(filter, "class", "network");

  osinfo_list_add(OSINFO_LIST(list1), OSINFO_ENTITY(ent1));
  osinfo_list_add(OSINFO_LIST(list1), OSINFO_ENTITY(ent2));
  osinfo_list_add(OSINFO_LIST(list1), OSINFO_ENTITY(ent3));
  osinfo_list_add(OSINFO_LIST(list1), OSINFO_ENTITY(ent4));

  list2 = osinfo_devicelist_new_filtered(list1, filter);

  fail_unless(osinfo_list_get_length(OSINFO_LIST(list2)) == 3, "List did not have 3 elements");

  gboolean has1 = FALSE;
  gboolean has2 = FALSE;
  gboolean has3 = FALSE;
  gboolean has4 = FALSE;
  gboolean hasBad = FALSE;
  int i;
  for (i = 0 ; i < osinfo_list_get_length(OSINFO_LIST(list2)) ; i++) {
    OsinfoDevice *ent = OSINFO_DEVICE(osinfo_list_get_nth(OSINFO_LIST(list2), i));
    if (ent == ent1)
      has1 = TRUE;
    else if (ent == ent2)
      has2 = TRUE;
    else if (ent == ent3)
      has3 = TRUE;
    else if (ent == ent4)
      has4 = TRUE;
    else
      hasBad = TRUE;
  }
  fail_unless(has1, "List was missing entity 1");
  fail_unless(has2, "List was missing entity 2");
  fail_unless(has3, "List was missing entity 3");
  fail_unless(!has4, "List had unexpected entity 4");
  fail_unless(!hasBad, "List had unexpected entity");

  g_object_unref(ent1);
  g_object_unref(ent2);
  g_object_unref(ent3);
  g_object_unref(ent4);
  g_object_unref(filter);
  g_object_unref(list1);
  g_object_unref(list2);
}
END_TEST


static Suite *
list_suite(void)
{
  Suite *s = suite_create("List");
  TCase *tc = tcase_create("Core");
  tcase_add_test(tc, test_union);
  tcase_add_test(tc, test_intersect);
  tcase_add_test(tc, test_filter);
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
  osinfo_device_get_type();
  osinfo_devicelist_get_type();
  osinfo_filter_get_type();

  srunner_run_all (sr, CK_ENV);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
