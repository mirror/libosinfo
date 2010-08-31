#include <stdlib.h>
#include <osinfo/osinfo.h>
#include <check.h>



START_TEST(test_basic)
{
    OsinfoOsfilter *osfilter = osinfo_osfilter_new();
    OsinfoOs *os1 = osinfo_os_new("pretty");
    OsinfoOs *os2 = osinfo_os_new("ugly");

    fail_unless(OSINFO_IS_OSFILTER(osfilter), "Osfilter is a osfilter object");
    fail_unless(osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os1)), "Osfilter matches OS");

    osinfo_osfilter_add_os_constraint(osfilter, OSINFO_OS_RELATIONSHIP_DERIVES_FROM, os1);
    GList *tmp = osinfo_osfilter_get_os_constraint_values(osfilter,
                                                          OSINFO_OS_RELATIONSHIP_DERIVES_FROM);
    fail_unless(tmp != NULL, "Unexpected missing OS");
    fail_unless(tmp->data == os1, "Derived OS is OS 1");
    fail_unless(tmp->next == NULL, "Too many derived OS");
    g_list_free(tmp);

    tmp = osinfo_osfilter_get_os_constraint_values(osfilter,
                                                   OSINFO_OS_RELATIONSHIP_CLONES);
    fail_unless(tmp == NULL, "Unexpected cloned OS");


    osinfo_osfilter_add_os_constraint(osfilter, OSINFO_OS_RELATIONSHIP_DERIVES_FROM, os2);
    tmp = osinfo_osfilter_get_os_constraint_values(osfilter,
                                                   OSINFO_OS_RELATIONSHIP_DERIVES_FROM);
    fail_unless(tmp != NULL, "Unexpected missing OS");
    fail_unless(tmp->data == os2, "Derived OS is OS 1");
    fail_unless(tmp->next != NULL, "Not enough derived OS");
    fail_unless(tmp->next->data == os1, "Derived OS is OS 1");
    fail_unless(tmp->next->next == NULL, "Too many derived OS");
    g_list_free(tmp);

    osinfo_osfilter_add_os_constraint(osfilter, OSINFO_OS_RELATIONSHIP_UPGRADES, os1);
    tmp = osinfo_osfilter_get_os_constraint_values(osfilter,
                                                   OSINFO_OS_RELATIONSHIP_UPGRADES);
    fail_unless(tmp != NULL, "Unexpected missing OS");
    fail_unless(tmp->data == os1, "Derived OS is OS 1");
    fail_unless(tmp->next == NULL, "Too many derived OS");
    g_list_free(tmp);

    osinfo_osfilter_add_os_constraint(osfilter, OSINFO_OS_RELATIONSHIP_CLONES, os1);
    tmp = osinfo_osfilter_get_os_constraint_values(osfilter,
                                                   OSINFO_OS_RELATIONSHIP_CLONES);
    fail_unless(tmp != NULL, "Unexpected missing OS");
    fail_unless(tmp->data == os1, "Derived OS is OS 1");
    fail_unless(tmp->next == NULL, "Too many derived OS");
    g_list_free(tmp);

    g_object_unref(os2);
    g_object_unref(os1);
    g_object_unref(osfilter);
}
END_TEST


START_TEST(test_osfilter_single)
{
    OsinfoOsfilter *osfilter = osinfo_osfilter_new();
    OsinfoOs *os1 = osinfo_os_new("hot");
    OsinfoOs *os2 = osinfo_os_new("or");
    OsinfoOs *os3 = osinfo_os_new("not");
    OsinfoOs *os4 = osinfo_os_new("burger");


    osinfo_os_add_related_os(os1, OSINFO_OS_RELATIONSHIP_DERIVES_FROM, os2);
    osinfo_os_add_related_os(os3, OSINFO_OS_RELATIONSHIP_DERIVES_FROM, os4);

    osinfo_osfilter_add_os_constraint(osfilter, OSINFO_OS_RELATIONSHIP_DERIVES_FROM, os2);
    fail_unless(osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os1)), "Filter matches OS 1");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os2)), "Filter does not match OS 2");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os3)), "Filter does not match OS 3");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os4)), "Filter does not match OS 4");

    g_object_unref(os1);
    g_object_unref(os2);
    g_object_unref(os3);
    g_object_unref(os4);
    g_object_unref(osfilter);
}
END_TEST


START_TEST(test_osfilter_multi)
{
    OsinfoOsfilter *osfilter = osinfo_osfilter_new();
    OsinfoOs *os1 = osinfo_os_new("hot");
    OsinfoOs *os2 = osinfo_os_new("or");
    OsinfoOs *os3 = osinfo_os_new("not");
    OsinfoOs *os4 = osinfo_os_new("burger");
    OsinfoOs *os5 = osinfo_os_new("cat");

    osinfo_os_add_related_os(os1, OSINFO_OS_RELATIONSHIP_DERIVES_FROM, os2);
    osinfo_os_add_related_os(os1, OSINFO_OS_RELATIONSHIP_CLONES, os5);
    osinfo_os_add_related_os(os3, OSINFO_OS_RELATIONSHIP_DERIVES_FROM, os2);
    osinfo_os_add_related_os(os3, OSINFO_OS_RELATIONSHIP_CLONES, os4);

    osinfo_osfilter_add_os_constraint(osfilter, OSINFO_OS_RELATIONSHIP_DERIVES_FROM, os2);
    osinfo_osfilter_add_os_constraint(osfilter, OSINFO_OS_RELATIONSHIP_CLONES, os5);

    fail_unless(osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os1)), "Filter matches OS 1");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os2)), "Filter does not match OS 2");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os3)), "Filter does not match OS 3");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os4)), "Filter does not match OS 4");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os5)), "Filter does not match OS 5");

    osinfo_osfilter_clear_os_constraint(osfilter, OSINFO_OS_RELATIONSHIP_CLONES);

    fail_unless(osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os1)), "Filter matches OS 1");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os2)), "Filter does not match OS 2");
    fail_unless(osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os3)), "Filter matches OS 3");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os4)), "Filter does not match OS 4");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os5)), "Filter does not match OS 5");

    osinfo_osfilter_clear_os_constraints(osfilter);

    osinfo_osfilter_add_os_constraint(osfilter, OSINFO_OS_RELATIONSHIP_UPGRADES, os5);
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os1)), "Filter does not match OS 1");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os2)), "Filter does not match OS 2");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os3)), "Filter does not match OS 3");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os4)), "Filter does not match OS 4");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os5)), "Filter does not match OS 5");

    g_object_unref(os1);
    g_object_unref(os2);
    g_object_unref(os3);
    g_object_unref(os4);
    g_object_unref(os5);
    g_object_unref(osfilter);
}
END_TEST


START_TEST(test_osfilter_combine)
{
    OsinfoOsfilter *osfilter = osinfo_osfilter_new();
    OsinfoOs *os1 = osinfo_os_new("hot");
    OsinfoOs *os2 = osinfo_os_new("or");
    OsinfoOs *os3 = osinfo_os_new("not");
    OsinfoOs *os4 = osinfo_os_new("burger");

    osinfo_os_add_related_os(os1, OSINFO_OS_RELATIONSHIP_DERIVES_FROM, os2);
    osinfo_os_add_related_os(os3, OSINFO_OS_RELATIONSHIP_DERIVES_FROM, os4);

    osinfo_entity_add_param(OSINFO_ENTITY(os1), "vendor", "drevil");
    osinfo_entity_add_param(OSINFO_ENTITY(os3), "vendor", "acme");

    osinfo_osfilter_add_os_constraint(osfilter, OSINFO_OS_RELATIONSHIP_DERIVES_FROM, os2);
    fail_unless(osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os1)), "Filter matches OS 1");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os2)), "Filter does not match OS 2");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os3)), "Filter does not match OS 3");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os4)), "Filter does not match OS 4");

    osinfo_filter_add_constraint(OSINFO_FILTER(osfilter), "vendor", "acme");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os1)), "Filter does not match OS 1");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os2)), "Filter does not match OS 2");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os3)), "Filter does not match OS 3");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os4)), "Filter does not match OS 4");

    osinfo_osfilter_clear_os_constraint(osfilter, OSINFO_OS_RELATIONSHIP_DERIVES_FROM);
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os1)), "Filter does not match OS 1");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os2)), "Filter does not match OS 2");
    fail_unless(osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os3)), "Filter matches OS 3");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(osfilter), OSINFO_ENTITY(os4)), "Filter does not match OS 4");

    g_object_unref(os1);
    g_object_unref(os2);
    g_object_unref(os3);
    g_object_unref(os4);
    g_object_unref(osfilter);
}
END_TEST


static Suite *
osfilter_suite(void)
{
    Suite *s = suite_create("Osfilter");
    TCase *tc = tcase_create("Core");
    tcase_add_test(tc, test_basic);
    tcase_add_test(tc, test_osfilter_single);
    tcase_add_test(tc, test_osfilter_multi);
    tcase_add_test(tc, test_osfilter_combine);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = osfilter_suite ();
    SRunner *sr = srunner_create (s);

    g_type_init();

    /* Upfront so we don't confuse valgrind */
    osinfo_entity_get_type();
    osinfo_filter_get_type();
    osinfo_list_get_type();
    osinfo_oslist_get_type();
    osinfo_osfilter_get_type();
    osinfo_os_get_type();

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
