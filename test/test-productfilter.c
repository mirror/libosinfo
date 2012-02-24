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

#include <stdlib.h>
#include <osinfo/osinfo.h>
#include <check.h>

/* OsinfoProduct is abstract, so we need to trivially subclass it to test it */
typedef struct _OsinfoDummy        OsinfoDummy;
typedef struct _OsinfoDummyClass   OsinfoDummyClass;

struct _OsinfoDummy
{
    OsinfoProduct parent_instance;
};

struct _OsinfoDummyClass
{
    OsinfoProductClass parent_class;
};

GType osinfo_dummy_get_type(void);

G_DEFINE_TYPE (OsinfoDummy, osinfo_dummy, OSINFO_TYPE_PRODUCT);

static void osinfo_dummy_class_init(OsinfoDummyClass *klass G_GNUC_UNUSED){}
static void osinfo_dummy_init (OsinfoDummy *self G_GNUC_UNUSED) {}

static OsinfoProduct *osinfo_dummy_new(const gchar *id) {
    return g_object_new(osinfo_dummy_get_type(), "id", id, NULL);
}


START_TEST(test_basic)
{
    OsinfoProductFilter *productfilter = osinfo_productfilter_new();
    OsinfoProduct *product1 = osinfo_dummy_new("pretty");
    OsinfoProduct *product2 = osinfo_dummy_new("ugly");

    fail_unless(OSINFO_IS_PRODUCTFILTER(productfilter), "ProductFilter is a productfilter object");
    fail_unless(osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product1)), "ProductFilter matches PRODUCT");

    osinfo_productfilter_add_product_constraint(productfilter, OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM, product1);
    GList *tmp = osinfo_productfilter_get_product_constraint_values(productfilter,
                                                               OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM);
    fail_unless(tmp != NULL, "Unexpected missing PRODUCT");
    fail_unless(tmp->data == product1, "Derived PRODUCT is PRODUCT 1");
    fail_unless(tmp->next == NULL, "Too many derived PRODUCT");
    g_list_free(tmp);

    tmp = osinfo_productfilter_get_product_constraint_values(productfilter,
                                                   OSINFO_PRODUCT_RELATIONSHIP_CLONES);
    fail_unless(tmp == NULL, "Unexpected cloned PRODUCT");


    osinfo_productfilter_add_product_constraint(productfilter, OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM, product2);
    tmp = osinfo_productfilter_get_product_constraint_values(productfilter,
                                                   OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM);
    fail_unless(tmp != NULL, "Unexpected missing PRODUCT");
    fail_unless(tmp->data == product2, "Derived PRODUCT is PRODUCT 1");
    fail_unless(tmp->next != NULL, "Not enough derived PRODUCT");
    fail_unless(tmp->next->data == product1, "Derived PRODUCT is PRODUCT 1");
    fail_unless(tmp->next->next == NULL, "Too many derived PRODUCT");
    g_list_free(tmp);

    osinfo_productfilter_add_product_constraint(productfilter, OSINFO_PRODUCT_RELATIONSHIP_UPGRADES, product1);
    tmp = osinfo_productfilter_get_product_constraint_values(productfilter,
                                                   OSINFO_PRODUCT_RELATIONSHIP_UPGRADES);
    fail_unless(tmp != NULL, "Unexpected missing PRODUCT");
    fail_unless(tmp->data == product1, "Derived PRODUCT is PRODUCT 1");
    fail_unless(tmp->next == NULL, "Too many derived PRODUCT");
    g_list_free(tmp);

    osinfo_productfilter_add_product_constraint(productfilter, OSINFO_PRODUCT_RELATIONSHIP_CLONES, product1);
    tmp = osinfo_productfilter_get_product_constraint_values(productfilter,
                                                   OSINFO_PRODUCT_RELATIONSHIP_CLONES);
    fail_unless(tmp != NULL, "Unexpected missing PRODUCT");
    fail_unless(tmp->data == product1, "Derived PRODUCT is PRODUCT 1");
    fail_unless(tmp->next == NULL, "Too many derived PRODUCT");
    g_list_free(tmp);

    g_object_unref(product2);
    g_object_unref(product1);
    g_object_unref(productfilter);
}
END_TEST


START_TEST(test_productfilter_single)
{
    OsinfoProductFilter *productfilter = osinfo_productfilter_new();
    OsinfoProduct *product1 = osinfo_dummy_new("hot");
    OsinfoProduct *product2 = osinfo_dummy_new("or");
    OsinfoProduct *product3 = osinfo_dummy_new("not");
    OsinfoProduct *product4 = osinfo_dummy_new("burger");


    osinfo_product_add_related(product1, OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM, product2);
    osinfo_product_add_related(product3, OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM, product4);

    osinfo_productfilter_add_product_constraint(productfilter, OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM, product2);
    fail_unless(osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product1)), "Filter matches PRODUCT 1");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product2)), "Filter does not match PRODUCT 2");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product3)), "Filter does not match PRODUCT 3");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product4)), "Filter does not match PRODUCT 4");

    g_object_unref(product1);
    g_object_unref(product2);
    g_object_unref(product3);
    g_object_unref(product4);
    g_object_unref(productfilter);
}
END_TEST


START_TEST(test_productfilter_multi)
{
    OsinfoProductFilter *productfilter = osinfo_productfilter_new();
    OsinfoProduct *product1 = osinfo_dummy_new("hot");
    OsinfoProduct *product2 = osinfo_dummy_new("or");
    OsinfoProduct *product3 = osinfo_dummy_new("not");
    OsinfoProduct *product4 = osinfo_dummy_new("burger");
    OsinfoProduct *product5 = osinfo_dummy_new("cat");

    osinfo_product_add_related(product1, OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM, product2);
    osinfo_product_add_related(product1, OSINFO_PRODUCT_RELATIONSHIP_CLONES, product5);
    osinfo_product_add_related(product3, OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM, product2);
    osinfo_product_add_related(product3, OSINFO_PRODUCT_RELATIONSHIP_CLONES, product4);

    osinfo_productfilter_add_product_constraint(productfilter, OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM, product2);
    osinfo_productfilter_add_product_constraint(productfilter, OSINFO_PRODUCT_RELATIONSHIP_CLONES, product5);

    fail_unless(osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product1)), "Filter matches PRODUCT 1");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product2)), "Filter does not match PRODUCT 2");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product3)), "Filter does not match PRODUCT 3");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product4)), "Filter does not match PRODUCT 4");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product5)), "Filter does not match PRODUCT 5");

    osinfo_productfilter_clear_product_constraint(productfilter, OSINFO_PRODUCT_RELATIONSHIP_CLONES);

    fail_unless(osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product1)), "Filter matches PRODUCT 1");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product2)), "Filter does not match PRODUCT 2");
    fail_unless(osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product3)), "Filter matches PRODUCT 3");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product4)), "Filter does not match PRODUCT 4");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product5)), "Filter does not match PRODUCT 5");

    osinfo_productfilter_clear_product_constraints(productfilter);

    osinfo_productfilter_add_product_constraint(productfilter, OSINFO_PRODUCT_RELATIONSHIP_UPGRADES, product5);
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product1)), "Filter does not match PRODUCT 1");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product2)), "Filter does not match PRODUCT 2");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product3)), "Filter does not match PRODUCT 3");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product4)), "Filter does not match PRODUCT 4");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product5)), "Filter does not match PRODUCT 5");

    g_object_unref(product1);
    g_object_unref(product2);
    g_object_unref(product3);
    g_object_unref(product4);
    g_object_unref(product5);
    g_object_unref(productfilter);
}
END_TEST


START_TEST(test_productfilter_combine)
{
    OsinfoProductFilter *productfilter = osinfo_productfilter_new();
    OsinfoProduct *product1 = osinfo_dummy_new("hot");
    OsinfoProduct *product2 = osinfo_dummy_new("or");
    OsinfoProduct *product3 = osinfo_dummy_new("not");
    OsinfoProduct *product4 = osinfo_dummy_new("burger");

    osinfo_product_add_related(product1, OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM, product2);
    osinfo_product_add_related(product3, OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM, product4);

    osinfo_entity_add_param(OSINFO_ENTITY(product1), "vendor", "drevil");
    osinfo_entity_add_param(OSINFO_ENTITY(product3), "vendor", "acme");

    osinfo_productfilter_add_product_constraint(productfilter, OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM, product2);
    fail_unless(osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product1)), "Filter matches PRODUCT 1");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product2)), "Filter does not match PRODUCT 2");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product3)), "Filter does not match PRODUCT 3");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product4)), "Filter does not match PRODUCT 4");

    osinfo_filter_add_constraint(OSINFO_FILTER(productfilter), "vendor", "acme");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product1)), "Filter does not match PRODUCT 1");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product2)), "Filter does not match PRODUCT 2");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product3)), "Filter does not match PRODUCT 3");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product4)), "Filter does not match PRODUCT 4");

    osinfo_productfilter_clear_product_constraint(productfilter, OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM);
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product1)), "Filter does not match PRODUCT 1");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product2)), "Filter does not match PRODUCT 2");
    fail_unless(osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product3)), "Filter matches PRODUCT 3");
    fail_unless(!osinfo_filter_matches(OSINFO_FILTER(productfilter), OSINFO_ENTITY(product4)), "Filter does not match PRODUCT 4");

    g_object_unref(product1);
    g_object_unref(product2);
    g_object_unref(product3);
    g_object_unref(product4);
    g_object_unref(productfilter);
}
END_TEST


static Suite *
productfilter_suite(void)
{
    Suite *s = suite_create("ProductFilter");
    TCase *tc = tcase_create("Core");
    tcase_add_test(tc, test_basic);
    tcase_add_test(tc, test_productfilter_single);
    tcase_add_test(tc, test_productfilter_multi);
    tcase_add_test(tc, test_productfilter_combine);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = productfilter_suite ();
    SRunner *sr = srunner_create (s);

    g_type_init();

    /* Upfront so we don't confuse valgrind */
    osinfo_entity_get_type();
    osinfo_filter_get_type();
    osinfo_list_get_type();
    osinfo_productlist_get_type();
    osinfo_productfilter_get_type();
    osinfo_product_get_type();

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
