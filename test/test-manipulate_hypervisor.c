#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <libosinfo.h>

int main(int argc, char** argv)
{
    int i, ret, num, err;
    osi_lib_t lib;
    osi_hypervisor_t hv;
    char** hv_all_properties;
    char** hv_all_values;
    char** hv_dev_sections;
    char* value;
    char* data_dir;

    assert ( argc == 2 );
    data_dir = argv[1];

    printf("Starting test...\n");
    printf("Checking to see if we got library handle...\n");
    lib = osi_get_lib_handle(&err, data_dir);
    assert ( err == 0 );

    /* Set hypervisors for libraries */
    printf("Setting hypervisor...\n");
    ret = osi_set_lib_hypervisor(lib, "http://qemu.org/qemu-kvm-0.11.0");
    assert ( ret == 0 );

    /* Initializing library */
    printf("Initializing library...\n");
    ret = osi_init_lib(lib);
    assert ( ret == 0 );

    /* Get handle to hypervisor */
    hv = osi_get_lib_hypervisor(lib, &err);
    assert ( err == 0 );

    /* Get all properties for the hypervisor. */
    hv_all_properties = osi_get_hv_all_property_keys(hv, &num, &err);
    assert ( err == 0 );
    assert ( num == 3 ); /* We control the test data */
    assert ( hv_all_properties != NULL );

    /* Check that name, version and updates-version properties are defined */
    printf("Checking for name property...\n");
    assert ( strcmp(hv_all_properties[0], "name") == 0 ||
             strcmp(hv_all_properties[1], "name") == 0 ||
             strcmp(hv_all_properties[2], "name") == 0 );
    printf("Checking for version property...\n");
    assert ( strcmp(hv_all_properties[0], "version") == 0 ||
             strcmp(hv_all_properties[1], "version") == 0 ||
             strcmp(hv_all_properties[2], "version") == 0 );
    printf("Checking for updates-version property...\n");
    assert ( strcmp(hv_all_properties[0], "updates-version") == 0 ||
             strcmp(hv_all_properties[1], "updates-version") == 0 ||
             strcmp(hv_all_properties[2], "updates-version") == 0 );

    free(hv_all_properties[0]);
    free(hv_all_properties[1]);
    free(hv_all_properties[2]);
    free(hv_all_properties);

    /* Now we check the first value for each property */
    printf("Checking value for name with first value...\n");
    value = osi_get_hv_property_first_value(hv, "name", &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp("KVM", value) == 0 );
    free(value);

    printf("Checking value for version with first value...\n");
    value = osi_get_hv_property_first_value(hv, "version", &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp("0.11.0", value) == 0 );
    free(value);

    printf("Checking value for updates-version with first value...\n");
    value = osi_get_hv_property_first_value(hv, "updates-version", &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp("0.10.0", value) == 0 );
    free(value);

    /* Now check all values for each property */
    printf("Checking all values for name...\n");
    hv_all_values = osi_get_hv_property_all_values(hv, "name", &num, &err);
    assert ( err == 0 );
    assert ( num == 1 );
    assert ( hv_all_values != NULL );
    assert ( strcmp(hv_all_values[0], "KVM") == 0 );
    for (i = 0; i < num; i++)
        free(hv_all_values[i]);
    free(hv_all_values);

    printf("Checking all values for version...\n");
    hv_all_values = osi_get_hv_property_all_values(hv, "version", &num, &err);
    assert ( err == 0 );
    assert ( num == 1 );
    assert ( hv_all_values != NULL );
    assert ( strcmp(hv_all_values[0], "0.11.0") == 0 );
    for (i = 0; i < num; i++)
        free(hv_all_values[i]);
    free(hv_all_values);

    printf("Checking all values for updates-version...\n");
    hv_all_values = osi_get_hv_property_all_values(hv, "updates-version", &num, &err);
    assert ( err == 0 );
    assert ( num == 3 );
    assert ( hv_all_values != NULL );
    assert ( strcmp(hv_all_values[0], "0.10.0") == 0 );
    assert ( strcmp(hv_all_values[1], "0.9.0") == 0 );
    assert ( strcmp(hv_all_values[2], "0.8.0") == 0 );
    for (i = 0; i < num; i++)
        free(hv_all_values[i]);
    free(hv_all_values);

    /* Checking device sections */
    printf("Checking defined device sections...\n");
    hv_dev_sections = osi_get_hv_device_types(hv, &num, &err);
    assert ( err == 0 );
    assert ( num == 2 );
    assert ( hv_dev_sections != NULL );
    assert ( strcmp(hv_dev_sections[0], "audio") == 0 ||
             strcmp(hv_dev_sections[1], "audio") == 0 );
    assert ( strcmp(hv_dev_sections[0], "network") == 0 ||
             strcmp(hv_dev_sections[1], "network") == 0 );
    for (i = 0; i < num; i++)
        free(hv_dev_sections[i]);
    free(hv_dev_sections);

    /* Dispose of hv handle */
    ret = osi_cleanup_handle(hv);
    assert ( ret == 0 );

    /* Close library */
    printf("Closing library...\n");
    ret = osi_close_lib(lib);
    assert ( ret == 0 );

    printf("test-single_hv_manipulate succeeded.\n");
    return 0;
}
