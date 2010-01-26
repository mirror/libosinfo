#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <libosinfo.h>

int main(void)
{
    int ret, len, err;
    osi_lib_t lib;
    osi_os_t os;
    osi_os_list_t os_list;
    osi_filter_t filter;
    char* value;

    printf("Starting test-get_os...\n");
    printf("Checking to see if we got library handle...\n");
    lib = osi_get_lib_handle(&err, "/home/arjun/Desktop/redhat/libosinfo/data");
    assert ( err == 0 );

    /* Initializing library */
    printf("Initializing library...\n");
    ret = osi_init_lib(lib);
    assert ( ret == 0 );

    /* First get all the OS objects that we know about */
    printf("Get list with all operating systems...\n");
    filter = NULL;
    os_list = osi_get_os_list(lib, filter, &err);
    assert ( err == 0 );
    assert ( osi_os_list_length(os_list) == 3 );

    printf("Verify that the expected operating systems are present...\n");
    os = osi_get_os_by_index(os_list, 0, &err);
    assert ( err == 0 );
    value = osi_get_os_id(os, &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp(value, "http://fedoraproject.org/fedora-11") == 0 );
    free(value);
    ret = osi_cleanup_handle(os);
    assert ( ret == 0 );

    os = osi_get_os_by_index(os_list, 1, &err);
    assert ( err == 0 );
    value = osi_get_os_id(os, &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp(value, "http://fedoraproject.org/fedora-10") == 0 );
    free(value);
    ret = osi_cleanup_handle(os);
    assert ( ret == 0 );

    os = osi_get_os_by_index(os_list, 2, &err);
    assert ( err == 0 );
    value = osi_get_os_id(os, &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp(value, "http://software.opensuse.org/112") == 0 );
    free(value);
    ret = osi_cleanup_handle(os);
    assert ( ret == 0 );

    /* Freeing os list */
    printf("Done with this list, freeing...\n");
    ret = osi_free_os_list(os_list);
    assert ( ret == 0 );

    /* Now test with filter */
    printf("Testing os list filtering...\n");
    filter = osi_get_filter(lib, &err);
    assert ( err == 0 );
    ret = osi_add_filter_constraint(filter, "vendor", "Fedora Project");
    assert ( ret == 0 );

    os_list = osi_get_os_list(lib, filter, &err);
    assert ( err == 0 );
    assert ( osi_os_list_length(os_list) == 2 );

    printf("Verify that the expected operating systems are present...\n");
    os = osi_get_os_by_index(os_list, 0, &err);
    assert ( err == 0 );
    value = osi_get_os_id(os, &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp(value, "http://fedoraproject.org/fedora-11") == 0 );
    free(value);
    ret = osi_cleanup_handle(os);
    assert ( ret == 0 );

    os = osi_get_os_by_index(os_list, 1, &err);
    assert ( err == 0 );
    value = osi_get_os_id(os, &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp(value, "http://fedoraproject.org/fedora-10") == 0 );
    free(value);
    ret = osi_cleanup_handle(os);
    assert ( ret == 0 );

    /* Freeing os list */
    printf("Done with this list, freeing...\n");
    ret = osi_free_os_list(os_list);
    assert ( ret == 0 );


    /* Get an OS by id so we can check relationship filtering */
    printf("Getting an os object to test more os/filter methods...\n");
    os = osi_get_os_by_id(lib, "http://fedoraproject.org/fedora-10", &err);
    assert ( err == 0 );

    /* Use Fedora10 os reference to find operating systems that upgrade it */
    printf("Testing filtering based on os relationships...\n");
    ret = osi_clear_all_constraints(filter);
    assert ( ret == 0 );
    ret = osi_add_relation_constraint(filter, UPGRADES, os);
    assert ( ret == 0 );

    /* Done with F10 reference so clear it */
    ret = osi_cleanup_handle(os);
    assert ( ret == 0 );

    /* Get the filtered by relationships os list */
    os_list = osi_get_os_list(lib, filter, &err);
    assert ( err == 0 );
    assert ( osi_os_list_length(os_list) == 1 );

    /* Free filter */
    ret = osi_free_filter(filter);
    assert ( ret == 0 );

    printf("Verify that the expected operating systems are present...\n");
    os = osi_get_os_by_index(os_list, 0, &err);
    value = osi_get_os_id(os, &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp(value, "http://fedoraproject.org/fedora-11") == 0 );
    free(value);
    ret = osi_cleanup_handle(os);
    assert ( ret == 0 );

    /* Freeing os list */
    printf("Done with this list, freeing...\n");
    ret = osi_free_os_list(os_list);
    assert ( ret == 0 );

    /* Close library */
    printf("Closing library...\n");
    ret = osi_close_lib(lib);
    assert ( ret == 0 );

    printf("test-get_os succeeded.\n");
    return 0;
}