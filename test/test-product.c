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
    OsinfoProduct *product = osinfo_dummy_new("pony");

    fail_unless(OSINFO_IS_PRODUCT(product), "Product is a product object");
    fail_unless(g_strcmp0(osinfo_entity_get_id(OSINFO_ENTITY(product)), "pony") == 0, "Product ID was pony");

    g_object_unref(product);
}
END_TEST


START_TEST(test_relproduct)
{
    OsinfoProduct *product1 = osinfo_dummy_new("pony");
    OsinfoProduct *product2 = osinfo_dummy_new("donkey");
    OsinfoProduct *product3 = osinfo_dummy_new("wathog");
    OsinfoProduct *product4 = osinfo_dummy_new("aardvark");
    OsinfoProduct *product5 = osinfo_dummy_new("unicorn");

    osinfo_product_add_related(product1, OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM, product2);
    osinfo_product_add_related(product1, OSINFO_PRODUCT_RELATIONSHIP_UPGRADES, product3);
    osinfo_product_add_related(product1, OSINFO_PRODUCT_RELATIONSHIP_UPGRADES, product4);
    osinfo_product_add_related(product3, OSINFO_PRODUCT_RELATIONSHIP_UPGRADES, product4);
    osinfo_product_add_related(product1, OSINFO_PRODUCT_RELATIONSHIP_CLONES, product5);

    OsinfoProductList *product1rel = osinfo_product_get_related(product1, OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM);
    fail_unless(osinfo_list_get_length(OSINFO_LIST(product1rel)) == 1, "Product has 1 derived product");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(product1rel), 0) == OSINFO_ENTITY(product2), "derived product is product2");
    g_object_unref(product1rel);

    product1rel = osinfo_product_get_related(product1, OSINFO_PRODUCT_RELATIONSHIP_UPGRADES);
    fail_unless(osinfo_list_get_length(OSINFO_LIST(product1rel)) == 2, "Product has 2 upgraded product");
    fail_unless((osinfo_list_get_nth(OSINFO_LIST(product1rel), 0) == OSINFO_ENTITY(product3) ||
                 osinfo_list_get_nth(OSINFO_LIST(product1rel), 0) == OSINFO_ENTITY(product4)) &&
                (osinfo_list_get_nth(OSINFO_LIST(product1rel), 1) == OSINFO_ENTITY(product3) ||
                 osinfo_list_get_nth(OSINFO_LIST(product1rel), 1) == OSINFO_ENTITY(product4)),
                "upgraded productes are product3 + product4");
    g_object_unref(product1rel);

    product1rel = osinfo_product_get_related(product3, OSINFO_PRODUCT_RELATIONSHIP_UPGRADES);
    fail_unless(osinfo_list_get_length(OSINFO_LIST(product1rel)) == 1, "Product has 1 upgraded product");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(product1rel), 0) == OSINFO_ENTITY(product4), "upgraded product is product4");
    g_object_unref(product1rel);

    product1rel = osinfo_product_get_related(product1, OSINFO_PRODUCT_RELATIONSHIP_CLONES);
    fail_unless(osinfo_list_get_length(OSINFO_LIST(product1rel)) == 1, "Product has 1 upgraded product");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(product1rel), 0) == OSINFO_ENTITY(product5), "cloned product is product5");
    g_object_unref(product1rel);

    g_object_unref(product1);
    g_object_unref(product2);
    g_object_unref(product3);
    g_object_unref(product4);
    g_object_unref(product5);
}
END_TEST



START_TEST(test_supportdate)
{
    OsinfoProductList *products = osinfo_productlist_new();
    OsinfoProduct *product1 = osinfo_dummy_new("pony");
    OsinfoProduct *product2 = osinfo_dummy_new("donkey");
    OsinfoProduct *product3 = osinfo_dummy_new("wathog");
    OsinfoProduct *product4 = osinfo_dummy_new("aardvark");
    OsinfoProductFilter *filter = osinfo_productfilter_new();
    OsinfoProductList *tmp;
    GDate *date;

    osinfo_list_add(OSINFO_LIST(products), OSINFO_ENTITY(product1));
    osinfo_list_add(OSINFO_LIST(products), OSINFO_ENTITY(product2));
    osinfo_list_add(OSINFO_LIST(products), OSINFO_ENTITY(product3));
    osinfo_list_add(OSINFO_LIST(products), OSINFO_ENTITY(product4));

    osinfo_entity_set_param(OSINFO_ENTITY(product2), OSINFO_PRODUCT_PROP_RELEASE_DATE, "2000-01-01");

    osinfo_entity_set_param(OSINFO_ENTITY(product3), OSINFO_PRODUCT_PROP_EOL_DATE, "2010-01-01");

    osinfo_entity_set_param(OSINFO_ENTITY(product4), OSINFO_PRODUCT_PROP_RELEASE_DATE, "2005-01-01");
    osinfo_entity_set_param(OSINFO_ENTITY(product4), OSINFO_PRODUCT_PROP_EOL_DATE, "2006-01-01");

    /* Product 1 & 3 */
    date = g_date_new_dmy(31, 12, 1999);
    osinfo_productfilter_add_support_date_constraint(filter, date);
    tmp = OSINFO_PRODUCTLIST(osinfo_list_new_filtered(OSINFO_LIST(products), OSINFO_FILTER(filter)));
    fail_unless(osinfo_list_get_length(OSINFO_LIST(tmp)) == 2, "2 products");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(tmp), 0) == (OsinfoEntity*)product1, "Got product 1");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(tmp), 1) == (OsinfoEntity*)product3, "Got product 3");
    g_object_unref(tmp);
    g_date_free(date);

    /* Product 1, 2 & 3 */
    date = g_date_new_dmy(01, 01, 2000);
    osinfo_productfilter_add_support_date_constraint(filter, date);
    tmp = OSINFO_PRODUCTLIST(osinfo_list_new_filtered(OSINFO_LIST(products), OSINFO_FILTER(filter)));
    fail_unless(osinfo_list_get_length(OSINFO_LIST(tmp)) == 3, "3 products");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(tmp), 0) == (OsinfoEntity*)product1, "Got product 1");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(tmp), 1) == (OsinfoEntity*)product2, "Got product 2");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(tmp), 2) == (OsinfoEntity*)product3, "Got product 3");
    g_object_unref(tmp);
    g_date_free(date);

    /* Product 1, 2 & 3 */
    date = g_date_new_dmy(01, 01, 2010);
    osinfo_productfilter_add_support_date_constraint(filter, date);
    tmp = OSINFO_PRODUCTLIST(osinfo_list_new_filtered(OSINFO_LIST(products), OSINFO_FILTER(filter)));
    fail_unless(osinfo_list_get_length(OSINFO_LIST(tmp)) == 3, "3 products");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(tmp), 0) == (OsinfoEntity*)product1, "Got product 1");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(tmp), 1) == (OsinfoEntity*)product2, "Got product 2");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(tmp), 2) == (OsinfoEntity*)product3, "Got product 3");
    g_object_unref(tmp);
    g_date_free(date);

    /* Product 1, 2 & 3 */
    date = g_date_new_dmy(01, 05, 2005);
    osinfo_productfilter_add_support_date_constraint(filter, date);
    tmp = OSINFO_PRODUCTLIST(osinfo_list_new_filtered(OSINFO_LIST(products), OSINFO_FILTER(filter)));
    fail_unless(osinfo_list_get_length(OSINFO_LIST(tmp)) == 4, "4 products");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(tmp), 0) == (OsinfoEntity*)product1, "Got product 1");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(tmp), 1) == (OsinfoEntity*)product2, "Got product 2");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(tmp), 2) == (OsinfoEntity*)product3, "Got product 3");
    fail_unless(osinfo_list_get_nth(OSINFO_LIST(tmp), 3) == (OsinfoEntity*)product4, "Got product 4");
    g_object_unref(tmp);
    g_date_free(date);

    g_object_unref(product1);
    g_object_unref(product2);
    g_object_unref(product3);
    g_object_unref(product4);
}
END_TEST



static Suite *
product_suite(void)
{
    Suite *s = suite_create("Product");
    TCase *tc = tcase_create("Core");
    tcase_add_test(tc, test_basic);
    tcase_add_test(tc, test_relproduct);
    tcase_add_test(tc, test_supportdate);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = product_suite ();
    SRunner *sr = srunner_create (s);

#if !GLIB_CHECK_VERSION(2,35,1)
    g_type_init();
#endif

    /* Upfront so we don't confuse valgrind */
    osinfo_platform_get_type();
    osinfo_device_get_type();
    osinfo_product_get_type();
    osinfo_productlist_get_type();
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
