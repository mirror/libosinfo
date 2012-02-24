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


START_TEST(test_id)
{
    OsinfoEntity *ent = g_object_new(osinfo_dummy_get_type(), "id", "myentity", NULL);
    gchar *id;

    fail_unless(g_strcmp0(osinfo_entity_get_id(ent), "myentity") == 0,
                "Entity id was not 'myentity'");

    g_object_get(ent, "id", &id, NULL);
    fail_unless(g_strcmp0(id, "myentity") == 0,
                "Entity id was not 'myentity'");
    g_free(id);

    g_object_unref(ent);
}
END_TEST

START_TEST(test_empty_props)
{
    OsinfoEntity *ent = g_object_new(osinfo_dummy_get_type(), "id", "myentity", NULL);

    GList *keys = osinfo_entity_get_param_keys(ent);
    fail_unless(keys == NULL, "Entity param key list was not empty");

    const gchar *value = osinfo_entity_get_param_value(ent, "wibble");
    fail_unless(value == NULL, "Entity param value was not NULL");

    GList *values = osinfo_entity_get_param_value_list(ent, "wibble");
    fail_unless(values == NULL, "Entity param value list was not NULL");

    g_object_unref(ent);
}
END_TEST

START_TEST(test_single_prop_value)
{
    OsinfoEntity *ent = g_object_new(osinfo_dummy_get_type(), "id", "myentity", NULL);

    osinfo_entity_add_param(ent, "hello", "world");

    GList *keys = osinfo_entity_get_param_keys(ent);
    fail_unless(keys != NULL, "Entity param key list was empty");
    fail_unless(keys->next == NULL, "Entity param key list has too many values");
    fail_unless(g_strcmp0(keys->data, "hello") == 0, "Entity param key was not 'hello'");
    g_list_free(keys);

    const gchar *value = osinfo_entity_get_param_value(ent, "hello");
    fail_unless(g_strcmp0(value, "world") == 0, "Entity param value was not 'world'");
    value = osinfo_entity_get_param_value(ent, "world");
    fail_unless(value == NULL, "Entity param bogus value was not NULL");

    GList *values = osinfo_entity_get_param_value_list(ent, "hello");
    fail_unless(values != NULL, "Entity param value list was empty");
    fail_unless(values->next == NULL, "Entity param value list has too many values");
    fail_unless(g_strcmp0(values->data, "world") == 0, "Entity param list value was not 'world'");
    g_list_free(values);

    g_object_unref(ent);
}
END_TEST

START_TEST(test_multi_prop_value)
{
    OsinfoEntity *ent = g_object_new(osinfo_dummy_get_type(), "id", "myentity", NULL);

    osinfo_entity_add_param(ent, "hello", "world");
    osinfo_entity_add_param(ent, "hello", "fred");
    osinfo_entity_add_param(ent, "hello", "elephant");

    GList *keys = osinfo_entity_get_param_keys(ent);
    fail_unless(keys != NULL, "Entity param key list was empty");
    fail_unless(keys->next == NULL, "Entity param key list has too many values");
    fail_unless(g_strcmp0(keys->data, "hello") == 0, "Entity param key was not 'hello'");
    g_list_free(keys);

    const gchar *value = osinfo_entity_get_param_value(ent, "hello");
    fail_unless(g_strcmp0(value, "world") == 0, "Entity param value was not 'world'");
    value = osinfo_entity_get_param_value(ent, "world");
    fail_unless(value == NULL, "Entity param bogus value was not NULL");

    GList *values = osinfo_entity_get_param_value_list(ent, "hello");
    fail_unless(values != NULL, "Entity param value list was empty");
    fail_unless(values->next != NULL, "Entity param value list doesn't have enough values");
    fail_unless(values->next->next != NULL, "Entity param value list doesn't have enough values");
    fail_unless(values->next->next->next == NULL, "Entity param value list has too many values");
    fail_unless(g_strcmp0(values->data, "world") == 0, "Entity param list first value was not 'world'");
    fail_unless(g_strcmp0(values->next->data, "fred") == 0, "Entity param list second value was not 'fred'");
    fail_unless(g_strcmp0(values->next->next->data, "elephant") == 0, "Entity param list third was not 'elephant'");
    g_list_free(values);

    g_object_unref(ent);
}
END_TEST

START_TEST(test_multi_props)
{
    OsinfoEntity *ent = g_object_new(osinfo_dummy_get_type(), "id", "myentity", NULL);

    osinfo_entity_add_param(ent, "hello", "world");
    osinfo_entity_add_param(ent, "fish", "food");
    osinfo_entity_add_param(ent, "kevin", "bacon");

    GList *keys = osinfo_entity_get_param_keys(ent);
    GList *tmp = keys;
    gboolean foundHello = FALSE;
    gboolean foundFish = FALSE;
    gboolean foundKevin = FALSE;
    gboolean foundBad = FALSE;
    while (tmp) {
        if (g_strcmp0(tmp->data, "hello") == 0)
            foundHello = TRUE;
        else if (g_strcmp0(tmp->data, "fish") == 0)
            foundFish = TRUE;
        else if (g_strcmp0(tmp->data, "kevin") == 0)
            foundKevin = TRUE;
        else
            foundBad = TRUE;
        tmp = tmp->next;
    }
    fail_unless(foundHello, "Entity param key list was missing 'hello'");
    fail_unless(foundFish, "Entity param key list was missing 'fish'");
    fail_unless(foundKevin, "Entity param key list was missing 'kevin'");
    fail_unless(!foundBad, "Entity param key list has unexpected key");
    g_list_free(keys);

    const gchar *value = osinfo_entity_get_param_value(ent, "hello");
    fail_unless(g_strcmp0(value, "world") == 0, "Entity param value was not 'world'");
    value = osinfo_entity_get_param_value(ent, "fish");
    fail_unless(g_strcmp0(value, "food") == 0, "Entity param value was not 'food'");
    value = osinfo_entity_get_param_value(ent, "kevin");
    fail_unless(g_strcmp0(value, "bacon") == 0, "Entity param value was not 'bacon'");

    GList *values = osinfo_entity_get_param_value_list(ent, "hello");
    fail_unless(values != NULL, "Entity param value list was empty");
    fail_unless(values->next == NULL, "Entity param value list has too many values");
    fail_unless(g_strcmp0(values->data, "world") == 0, "Entity param list value was not 'world'");
    g_list_free(values);

    values = osinfo_entity_get_param_value_list(ent, "fish");
    fail_unless(values != NULL, "Entity param value list was empty");
    fail_unless(values->next == NULL, "Entity param value list has too many values");
    fail_unless(g_strcmp0(values->data, "food") == 0, "Entity param list value was not 'food'");
    g_list_free(values);

    values = osinfo_entity_get_param_value_list(ent, "kevin");
    fail_unless(values != NULL, "Entity param value list was empty");
    fail_unless(values->next == NULL, "Entity param value list has too many values");
    fail_unless(g_strcmp0(values->data, "bacon") == 0, "Entity param list value was not 'bacon'");
    g_list_free(values);

    g_object_unref(ent);
}
END_TEST


START_TEST(test_multi_props_clear)
{
    OsinfoEntity *ent = g_object_new(osinfo_dummy_get_type(), "id", "myentity", NULL);

    osinfo_entity_add_param(ent, "hello", "world");
    osinfo_entity_add_param(ent, "fish", "food");

    GList *keys = osinfo_entity_get_param_keys(ent);
    GList *tmp = keys;
    gboolean foundHello = FALSE;
    gboolean foundFish = FALSE;
    gboolean foundBad = FALSE;
    while (tmp) {
        if (g_strcmp0(tmp->data, "hello") == 0)
            foundHello = TRUE;
        else if (g_strcmp0(tmp->data, "fish") == 0)
            foundFish = TRUE;
        else
            foundBad = TRUE;
        tmp = tmp->next;
    }
    fail_unless(foundHello, "Entity param key list was missing 'hello'");
    fail_unless(foundFish, "Entity param key list was missing 'fish'");
    fail_unless(!foundBad, "Entity param key list has unexpected key");
    g_list_free(keys);

    const gchar *value = osinfo_entity_get_param_value(ent, "hello");
    fail_unless(g_strcmp0(value, "world") == 0, "Entity param value was not 'world'");
    value = osinfo_entity_get_param_value(ent, "fish");
    fail_unless(g_strcmp0(value, "food") == 0, "Entity param value was not 'food'");

    osinfo_entity_clear_param(ent, "hello");

    keys = osinfo_entity_get_param_keys(ent);
    tmp = keys;
    foundHello = FALSE;
    foundFish = FALSE;
    foundBad = FALSE;
    while (tmp) {
        if (g_strcmp0(tmp->data, "hello") == 0)
            foundHello = TRUE;
        else if (g_strcmp0(tmp->data, "fish") == 0)
            foundFish = TRUE;
        else
            foundBad = TRUE;
        tmp = tmp->next;
    }
    fail_unless(!foundHello, "Entity param key list has unexpected 'hello'");
    fail_unless(foundFish, "Entity param key list was missing 'fish'");
    fail_unless(!foundBad, "Entity param key list has unexpected key");
    g_list_free(keys);

    value = osinfo_entity_get_param_value(ent, "hello");
    fail_unless(g_strcmp0(value, NULL) == 0, "Entity param value was not removed");
    value = osinfo_entity_get_param_value(ent, "fish");
    fail_unless(g_strcmp0(value, "food") == 0, "Entity param value was not 'food'");

    osinfo_entity_add_param(ent, "hello", "world");
    osinfo_entity_clear_param(ent, "fish");

    keys = osinfo_entity_get_param_keys(ent);
    tmp = keys;
    foundHello = FALSE;
    foundFish = FALSE;
    foundBad = FALSE;
    while (tmp) {
        if (g_strcmp0(tmp->data, "hello") == 0)
            foundHello = TRUE;
        else if (g_strcmp0(tmp->data, "fish") == 0)
            foundFish = TRUE;
        else
            foundBad = TRUE;
        tmp = tmp->next;
    }
    fail_unless(foundHello, "Entity param key list has unexpected 'hello'");
    fail_unless(!foundFish, "Entity param key list was missing 'fish'");
    fail_unless(!foundBad, "Entity param key list has unexpected key");
    g_list_free(keys);

    value = osinfo_entity_get_param_value(ent, "hello");
    fail_unless(g_strcmp0(value, "world") == 0, "Entity param value was not readded");
    value = osinfo_entity_get_param_value(ent, "fish");
    fail_unless(g_strcmp0(value, NULL) == 0, "Entity param value was not removed");

    g_object_unref(ent);
}
END_TEST


static Suite *
entity_suite(void)
{
    Suite *s = suite_create("Entity");
    TCase *tc = tcase_create("Core");
    tcase_add_test(tc, test_id);
    tcase_add_test(tc, test_empty_props);
    tcase_add_test(tc, test_single_prop_value);
    tcase_add_test(tc, test_multi_prop_value);
    tcase_add_test(tc, test_multi_props);
    tcase_add_test(tc, test_multi_props_clear);
    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = entity_suite ();
    SRunner *sr = srunner_create (s);

    g_type_init();

    /* Upfront so we don't confuse valgrind */
    osinfo_dummy_get_type();

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
