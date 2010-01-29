#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <libosinfo.h>

int main(int argc, char** argv)
{
    int i, ret, len, err, num;
    osi_lib_t lib;
    osi_os_t os, related_os;
    osi_os_list_t os_list;
    char* value;
    char** all_properties;
    char** all_values;
    char** unique_values;
    char* data_dir;

    assert ( argc == 2 );
    data_dir = argv[1];

    printf("Starting test-get_os...\n");
    printf("Checking to see if we got library handle...\n");
    lib = osi_get_lib_handle(&err, data_dir);
    assert ( err == 0 );

    /* Initializing library */
    printf("Initializing library...\n");
    ret = osi_init_lib(lib);
    assert ( ret == 0 );

    /* Get OS Handle */
    printf("Acquiring os handle...\n");
    os = osi_get_os_by_id(lib, "http://fedoraproject.org/fedora-11", &err);
    assert ( err == 0 );

    /* Verify handle has correct id */
    printf("Verifying os id...\n");
    value = osi_get_os_id(os, &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp("http://fedoraproject.org/fedora-11", value) == 0 );
    free(value);

    /* Check set properties for this os */
    printf("Checking set properties for os...\n");
    all_properties = osi_get_all_os_property_keys(os, &num, &err);
    assert ( err == 0 );
    assert ( num == 3 );
    assert ( all_properties != NULL );
    assert ( strcmp(all_properties[0], "short-id") == 0 );
    assert ( strcmp(all_properties[1], "name") == 0 );
    assert ( strcmp(all_properties[2], "vendor") == 0 );

    /* Free the properties, we're done with them */
    free(all_properties[0]);
    free(all_properties[1]);
    free(all_properties[2]);
    free(all_properties);

    /* Check multiple valued property */
    printf("Checking multiple valued property...\n");
    all_values = osi_get_os_property_all_values(os, "vendor", &len, &err);
    assert ( err == 0 );
    assert ( len == 2 );
    assert ( all_values != NULL );
    assert ( strcmp(all_values[0], "Fedora Project") == 0 );
    assert ( strcmp(all_values[1], "Red Hat") == 0 );
    free(all_values[0]);
    free(all_values[1]);
    free(all_values);

    value = osi_get_os_property_first_value(os, "vendor", &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp(value, "Fedora Project") == 0 );
    free(value);

    /* Check single valued property */
    printf("Checking single valued property...\n");
    value = osi_get_os_property_first_value(os, "short-id", &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp(value, "fedora11") == 0 );
    free(value);

    all_values = osi_get_os_property_all_values(os, "short-id", &len, &err);
    assert ( err == 0 );
    assert ( len == 1 );
    assert ( all_values != NULL );
    assert ( strcmp(all_values[0], "fedora11") == 0 );
    free(all_values[0]);
    free(all_values);

    /* Check unique values across all os objects for given property */
    printf("Checking unique values for given properties");
    unique_values = osi_unique_property_values(lib, "vendor", &len, &err);
    assert ( err == 0 );
    assert ( len == 3 );
    assert ( unique_values != NULL );
    for (i = 0; i < len; i++) {
        assert (
            strcmp("Fedora Project", unique_values[i]) == 0 ||
            strcmp("OpenSuse Project", unique_values[i]) == 0 ||
            strcmp("Red Hat", unique_values[i]) == 0
        );
    }
    free(unique_values[0]);
    free(unique_values[1]);
    free(unique_values[2]);
    free(unique_values);

    /* Check related OS */
    printf("Checking related os...\n");
    related_os = osi_get_related_os(os, DERIVES_FROM, &err);
    assert ( err == -EINVAL );
    assert ( !related_os );

    related_os = osi_get_related_os(os, UPGRADES, &err);
    assert ( err == 0 );
    assert ( related_os );

    value = osi_get_os_id(related_os, &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp(value, "http://fedoraproject.org/fedora-10") == 0 );
    free(value);

    ret = osi_cleanup_handle(related_os);
    assert ( ret == 0 );

    printf("Checking unique relationship values...\n");
    os_list = osi_unique_relationship_values(lib, CLONES, &err);
    assert ( err == 0 );
    len = osi_os_list_length(os_list);
    assert ( len == 0 );
    ret = osi_free_os_list(os_list);
    assert ( ret == 0 );

    os_list = osi_unique_relationship_values(lib, UPGRADES, &err);
    assert ( err == 0 );
    len = osi_os_list_length(os_list);
    assert ( len == 1 );
    related_os = osi_get_os_by_index(os_list, 0, &err);
    assert ( err == 0 );
    value = osi_get_os_id(related_os, &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp(value, "http://fedoraproject.org/fedora-11") == 0 );
    free(value);
    ret = osi_free_os_list(os_list);
    assert ( ret == 0 );

    /* Dispose of os handles */
    ret = osi_cleanup_handle(os);
    assert ( ret == 0 );
    ret = osi_cleanup_handle(related_os);
    assert ( ret == 0 );

    /* Close library */
    printf("Closing library...\n");
    ret = osi_close_lib(lib);
    assert ( ret == 0 );

    printf("test-manipulate_os succeeded.\n");
    return 0;
}
