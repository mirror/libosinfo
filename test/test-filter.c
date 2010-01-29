#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <libosinfo.h>

int main(int argc, char** argv)
{
    int ret, len, err;
    osi_lib_t lib;
    osi_filter_t filter;
    osi_os_t os, os_check;
    char** filter_properties;
    char* value, * os_id, * os_check_id;
    char* data_dir;

    assert ( argc == 2 );
    data_dir = argv[1];

    printf("Starting test-filter...\n");
    printf("Checking to see if we got library handle...\n");
    lib = osi_get_lib_handle(&err, data_dir);
    assert ( err == 0 );

    /* Initializing library */
    printf("Initializing library...\n");
    ret = osi_init_lib(lib);
    assert ( ret == 0 );

    /* Ensure that freeing an uninitialized filter is invalid */
    printf("Checking that freeing unitialized filter is invalid...\n");
    memset (&filter, sizeof filter, 0);
    ret = osi_free_filter(filter);
    assert ( ret != 0 );

    /* Get the filter */
    printf("Getting filter...\n");
    filter = osi_get_filter(lib, &err);
    assert ( err == 0 );

    /* Add bad filter constraints */
    printf("Adding bad constraints...\n");
    ret = osi_add_filter_constraint(filter, "", "0x1274");
    assert ( ret != 0 );
    ret = osi_add_filter_constraint(filter, "vendor", "");
    assert ( ret != 0 );
    ret = osi_add_filter_constraint(filter, NULL, "0x1274");
    assert ( ret != 0 );
    ret = osi_add_filter_constraint(filter, "vendor", NULL);
    assert ( ret != 0 );

    /* Add constraints */
    printf("Adding constraints...\n");
    ret = osi_add_filter_constraint(filter, "vendor", "0x1274");
    assert ( ret == 0 );
    ret = osi_add_filter_constraint(filter, "vendor", "0x1274");
    assert ( ret == 0 );
    ret = osi_add_filter_constraint(filter, "bus-type", "pci");
    assert ( ret == 0 );

    /* Check that the appropriate constraints were set */
    printf("Double checking that constraints are good...\n");
    filter_properties =  osi_get_filter_constraint_keys(filter, &len, &err);
    assert ( err == 0 );
    assert ( len == 2 );
    assert ( filter_properties != NULL );
    assert ( strcmp(filter_properties[0], "vendor") == 0 );
    assert ( strcmp(filter_properties[1], "bus-type") == 0 );
    free(filter_properties[0]);
    free(filter_properties[1]);
    free(filter_properties);

    value = osi_get_filter_constraint_value(filter, "vendor", &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp(value, "0x1274") == 0 );
    free(value);

    value = osi_get_filter_constraint_value(filter, "bus-type", &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp(value, "pci") == 0 );
    free(value);

    /* Test adding a relationship constraint with a bad os parameter */
    printf("Test adding a relationship constraint with a bad os parameter...\n");
    ret = osi_add_relation_constraint(filter, DERIVES_FROM, os);
    assert ( ret != 0 );

    /* Get an OS by id so we can add relationship filters as well */
    printf("Getting an os object to test adding relationship filter...\n");
    os = osi_get_os_by_id(lib, "http://fedoraproject.org/fedora-11", &err);
    assert ( err == 0 );

    /* Add a relationship constraint with this os */
    printf("Test adding a relationship constraint with a valid os parameter...\n");
    ret = osi_add_relation_constraint(filter, DERIVES_FROM, os);
    assert ( ret == 0 );

    /* Check the relationship constraint we didn't add */
    printf("Testing an unadded relationship constraint...\n");
    os_check = osi_get_relationship_constraint_value(filter, CLONES, &err);
    assert ( err == 0 );
    assert ( os_check == NULL );

    /* Check the relationship constraint we added */
    printf("Testing relationship constraints...\n");
    os_check = osi_get_relationship_constraint_value(filter, DERIVES_FROM, &err);
    assert ( err == 0 );

    os_id = osi_get_os_id(os, &err);
    assert ( err == 0 );
    assert ( os_id != NULL );

    os_check_id = osi_get_os_id(os_check, &err);
    assert ( err == 0 );
    assert ( os_check_id != NULL );

    assert ( strcmp(os_id, os_check_id) == 0 );
    free(os_id);
    free(os_check_id);

    /* Clear constraints */
    printf("Clearing non-existent constraint...\n");
    ret = osi_clear_filter_constraint(filter, "bad-param");
    assert ( ret != 0 );

    /* Clear filter constraint and check */
    printf("Clearing filter constraint...\n");
    ret = osi_clear_filter_constraint(filter, "vendor");
    assert ( ret == 0 );
    value = osi_get_filter_constraint_value(filter, "vendor", &err);
    assert ( err == 0 );
    assert ( value == NULL );

    /* Clear relationship constraint and check */
    printf("Clearing relationship constraint...\n");
    ret = osi_clear_relation_constraint(filter, DERIVES_FROM);
    assert ( ret == 0 );
    os_check = osi_get_relationship_constraint_value(filter, DERIVES_FROM, &err);
    assert ( err == 0 );
    assert ( os_check == NULL );

    /* Clear all remaining constraints and check */
    printf("Clearing all remaining relationship constraints...\n");
    ret = osi_clear_all_constraints(filter);
    assert ( ret == 0 );
    value = osi_get_filter_constraint_value(filter, "bus-type", &err);
    assert ( err == 0 );
    assert ( value == NULL );
    ret = osi_clear_filter_constraint(filter, "bus-type");
    assert ( ret != 0 );

    /* Free the filter */
    printf("Freeing filter...\n");
    ret = osi_free_filter(filter);
    assert ( ret == 0 );

    /* Close library */
    printf("Closing library...\n");
    ret = osi_close_lib(lib);
    assert ( ret == 0 );

    printf("test-filter succeeded.\n");
    return 0;
}
