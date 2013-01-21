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

/* OsinfoEntity is abstract, so we need to trivially subclass it to test it */
typedef struct _OsinfoDummy        OsinfoDummy;
typedef struct _OsinfoDummyClass   OsinfoDummyClass;

struct _OsinfoDummy
{
    OsinfoEntity parent_instance;
};

struct _OsinfoDummyClass
{
    OsinfoEntityClass parent_class;
};

GType osinfo_dummy_get_type(void);

G_DEFINE_TYPE (OsinfoDummy, osinfo_dummy, OSINFO_TYPE_ENTITY);

static void osinfo_dummy_class_init(OsinfoDummyClass *klass G_GNUC_UNUSED){}
static void osinfo_dummy_init (OsinfoDummy *self G_GNUC_UNUSED) {}


typedef struct _OsinfoDummyList        OsinfoDummyList;
typedef struct _OsinfoDummyListClass   OsinfoDummyListClass;

struct _OsinfoDummyList
{
    OsinfoList parent_instance;
};

struct _OsinfoDummyListClass
{
    OsinfoListClass parent_class;
};

GType osinfo_dummy_list_get_type(void);

G_DEFINE_TYPE (OsinfoDummyList, osinfo_dummy_list, OSINFO_TYPE_LIST);

static void osinfo_dummy_list_class_init(OsinfoDummyListClass *klass G_GNUC_UNUSED){}
static void osinfo_dummy_list_init (OsinfoDummyList *self G_GNUC_UNUSED) {}



START_TEST(test_basic)
{
    OsinfoList *list = g_object_new(osinfo_dummy_list_get_type(), NULL);

    fail_unless(osinfo_list_get_length(list) == 0, "List was not empty");
    fail_unless(osinfo_list_find_by_id(list, "wibble") == NULL, "List was not empty");

    GType type;
    g_object_get(list, "element-type", &type, NULL);
    fail_unless(type == OSINFO_TYPE_ENTITY, "Type is not entity");

    type = osinfo_list_get_element_type(list);
    fail_unless(type == OSINFO_TYPE_ENTITY, "Type is not entity");

    g_object_unref(list);
}
END_TEST



START_TEST(test_lookup)
{
    OsinfoList *list = g_object_new(osinfo_dummy_list_get_type(), NULL);
    OsinfoEntity *ent = g_object_new(osinfo_dummy_get_type(), "id", "wibble", NULL);

    osinfo_list_add(list, ent);

    fail_unless(osinfo_list_get_length(list) == 1, "List was empty");
    fail_unless(osinfo_list_get_nth(list, 0) == ent, "Lookup wrong element");
    fail_unless(osinfo_list_find_by_id(list, "wibble") != NULL, "Could not find element");
    fail_unless(osinfo_list_find_by_id(list, "fish") == NULL, "Found wrong element");

    g_object_unref(ent);
    g_object_unref(list);
}
END_TEST


START_TEST(test_union)
{
    OsinfoList *list1 = g_object_new(osinfo_dummy_list_get_type(), NULL);
    OsinfoList *list2 = g_object_new(osinfo_dummy_list_get_type(), NULL);
    OsinfoList *list3 = g_object_new(osinfo_dummy_list_get_type(), NULL);
    OsinfoEntity *ent1 = g_object_new(osinfo_dummy_get_type(), "id", "wibble1", NULL);
    OsinfoEntity *ent2 = g_object_new(osinfo_dummy_get_type(), "id", "wibble2", NULL);
    OsinfoEntity *ent3 = g_object_new(osinfo_dummy_get_type(), "id", "wibble3", NULL);
    OsinfoEntity *ent4 = g_object_new(osinfo_dummy_get_type(), "id", "wibble4", NULL);

    osinfo_list_add(list1, ent1);
    osinfo_list_add(list1, ent2);
    osinfo_list_add(list1, ent3);

    osinfo_list_add(list2, ent1);
    osinfo_list_add(list2, ent4);

    osinfo_list_add_union(list3, list1, list2);

    fail_unless(osinfo_list_get_length(list3) == 4, "List did not have 4 elements");

    gboolean has1 = FALSE;
    gboolean has2 = FALSE;
    gboolean has3 = FALSE;
    gboolean has4 = FALSE;
    gboolean hasBad = FALSE;
    int i;
    for (i = 0 ; i < osinfo_list_get_length(list3) ; i++) {
        OsinfoEntity *ent = osinfo_list_get_nth(list3, i);
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
    OsinfoList *list1 = g_object_new(osinfo_dummy_list_get_type(), NULL);
    OsinfoList *list2 = g_object_new(osinfo_dummy_list_get_type(), NULL);
    OsinfoList *list3 = g_object_new(osinfo_dummy_list_get_type(), NULL);
    OsinfoEntity *ent1 = g_object_new(osinfo_dummy_get_type(), "id", "wibble1", NULL);
    OsinfoEntity *ent2 = g_object_new(osinfo_dummy_get_type(), "id", "wibble2", NULL);
    OsinfoEntity *ent3 = g_object_new(osinfo_dummy_get_type(), "id", "wibble3", NULL);
    OsinfoEntity *ent4 = g_object_new(osinfo_dummy_get_type(), "id", "wibble4", NULL);

    osinfo_list_add(list1, ent1);
    osinfo_list_add(list1, ent2);
    osinfo_list_add(list1, ent3);

    osinfo_list_add(list2, ent1);
    osinfo_list_add(list2, ent3);
    osinfo_list_add(list2, ent4);

    osinfo_list_add_intersection(list3, list1, list2);

    fail_unless(osinfo_list_get_length(list3) == 2, "List did not have 2 elements");

    gboolean has1 = FALSE;
    gboolean has2 = FALSE;
    gboolean has3 = FALSE;
    gboolean has4 = FALSE;
    gboolean hasBad = FALSE;
    int i;
    for (i = 0 ; i < osinfo_list_get_length(list3) ; i++) {
        OsinfoEntity *ent = osinfo_list_get_nth(list3, i);
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
    OsinfoList *list1 = g_object_new(osinfo_dummy_list_get_type(), NULL);
    OsinfoList *list2 = g_object_new(osinfo_dummy_list_get_type(), NULL);
    OsinfoFilter *filter = osinfo_filter_new();
    OsinfoEntity *ent1 = g_object_new(osinfo_dummy_get_type(), "id", "wibble1", NULL);
    OsinfoEntity *ent2 = g_object_new(osinfo_dummy_get_type(), "id", "wibble2", NULL);
    OsinfoEntity *ent3 = g_object_new(osinfo_dummy_get_type(), "id", "wibble3", NULL);
    OsinfoEntity *ent4 = g_object_new(osinfo_dummy_get_type(), "id", "wibble4", NULL);

    osinfo_entity_add_param(ent1, "class", "network");
    osinfo_entity_add_param(ent1, "class", "wilma");
    osinfo_entity_add_param(ent2, "class", "network");
    osinfo_entity_add_param(ent3, "class", "network");
    osinfo_entity_add_param(ent3, "class", "audio");
    osinfo_entity_add_param(ent4, "class", "audio");

    osinfo_filter_add_constraint(filter, "class", "network");

    osinfo_list_add(list1, ent1);
    osinfo_list_add(list1, ent2);
    osinfo_list_add(list1, ent3);
    osinfo_list_add(list1, ent4);

    osinfo_list_add_filtered(list2, list1, filter);

    fail_unless(osinfo_list_get_length(list2) == 3, "List did not have 3 elements");

    gboolean has1 = FALSE;
    gboolean has2 = FALSE;
    gboolean has3 = FALSE;
    gboolean has4 = FALSE;
    gboolean hasBad = FALSE;
    int i;
    for (i = 0 ; i < osinfo_list_get_length(list2) ; i++) {
        OsinfoEntity *ent = osinfo_list_get_nth(list2, i);
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

struct iterateData {
    OsinfoEntity *ent1;
    OsinfoEntity *ent2;
    OsinfoEntity *ent3;
    OsinfoEntity *ent4;
    gboolean has1;
    gboolean has2;
    gboolean has3;
    gboolean has4;
    gboolean hasBad;
};

static void iterator(gpointer data, gpointer opaque)
{
    OsinfoEntity *ent = OSINFO_ENTITY(data);
    struct iterateData *idata = opaque;

    if (idata->ent1 == ent)
        idata->has1 = TRUE;
    else if (idata->ent2 == ent)
        idata->has2 = TRUE;
    else if (idata->ent3 == ent)
        idata->has3 = TRUE;
    else if (idata->ent4 == ent)
        idata->has4 = TRUE;
    else
        idata->hasBad = TRUE;
}

START_TEST(test_iterate)
{
    OsinfoList *list1 = g_object_new(osinfo_dummy_list_get_type(), NULL);
    OsinfoList *list2 = g_object_new(osinfo_dummy_list_get_type(), NULL);
    OsinfoEntity *ent1 = g_object_new(osinfo_dummy_get_type(), "id", "wibble1", NULL);
    OsinfoEntity *ent2 = g_object_new(osinfo_dummy_get_type(), "id", "wibble2", NULL);
    OsinfoEntity *ent3 = g_object_new(osinfo_dummy_get_type(), "id", "wibble3", NULL);
    OsinfoEntity *ent4 = g_object_new(osinfo_dummy_get_type(), "id", "wibble4", NULL);

    osinfo_list_add(list1, ent1);
    osinfo_list_add(list1, ent2);
    osinfo_list_add(list1, ent3);

    osinfo_list_add(list2, ent1);
    osinfo_list_add(list2, ent4);

    struct iterateData data = {
        ent1, ent2, ent3, ent4,
        FALSE, FALSE, FALSE, FALSE, FALSE
    };
    GList *elements = osinfo_list_get_elements(list1);
    g_list_foreach(elements, iterator, &data);
    g_list_free(elements);
    fail_unless(data.has1, "List was missing entity 1");
    fail_unless(data.has2, "List was missing entity 2");
    fail_unless(data.has3, "List was missing entity 3");
    fail_unless(!data.has4, "List has unexpected entity 4");
    fail_unless(!data.hasBad, "List had unexpected entity");

    data.has1 = data.has2 = data.has3 = data.has4 = data.hasBad = FALSE;

    elements = osinfo_list_get_elements(list2);
    g_list_foreach(elements, iterator, &data);
    g_list_free(elements);
    fail_unless(data.has1, "List was missing entity 1");
    fail_unless(!data.has2, "List has unexpected entity 2");
    fail_unless(!data.has3, "List has unexpected entity 3");
    fail_unless(data.has4, "List was missing entity 4");
    fail_unless(!data.hasBad, "List had unexpected entity");

    g_object_unref(ent1);
    g_object_unref(ent2);
    g_object_unref(ent3);
    g_object_unref(ent4);
    g_object_unref(list1);
    g_object_unref(list2);
}
END_TEST

static Suite *
list_suite(void)
{
    Suite *s = suite_create("List");
    TCase *tc = tcase_create("Core");
    tcase_add_test(tc, test_basic);
    tcase_add_test(tc, test_lookup);
    tcase_add_test(tc, test_union);
    tcase_add_test(tc, test_intersect);
    tcase_add_test(tc, test_filter);
    tcase_add_test(tc, test_iterate);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = list_suite ();
    SRunner *sr = srunner_create (s);

#if !GLIB_CHECK_VERSION(2,35,1)
    g_type_init();
#endif

    /* Upfront so we don't confuse valgrind */
    osinfo_dummy_get_type();
    osinfo_dummy_list_get_type();
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
