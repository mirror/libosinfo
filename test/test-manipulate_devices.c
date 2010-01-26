#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <libosinfo.h>

int main(int argc, char** argv)
{
    int ret, err, len;
    osi_lib_t lib;
    osi_device_t device;
    osi_os_t f10, f11;
    char** all_properties;
    char** all_values;
    char* value;
    char* f10_driver, * f11_driver;
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

    /* Get device by ID */
    printf("Acquiring device handle...\n");
    device = osi_get_device_by_id(lib, "http://pci-ids.ucw.cz/read/PC/1274/5000", &err);
    assert ( err == 0 );

    /* Check that ID is sane */
    printf("Double checking device id...\n");
    value = osi_device_id(device, &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp(value, "http://pci-ids.ucw.cz/read/PC/1274/5000") == 0 );
    free(value);

    /* Get an OS by id so we can check device driver */
    printf("Getting an os object to test device driver property...\n");
    f10 = osi_get_os_by_id(lib, "http://fedoraproject.org/fedora-10", &err);
    assert ( err == 0 );
    f11 = osi_get_os_by_id(lib, "http://fedoraproject.org/fedora-11", &err);
    assert ( err == 0 );

    /* Check all defined properties for device */
    printf("Checking all defined properties for device...\n");
    all_properties = osi_get_all_device_property_keys(device, &len, &err);
    assert ( err == 0 );
    assert ( len == 5 );
    assert ( all_properties != NULL );

    assert( strcmp("class", all_properties[0]) == 0 );
    assert( strcmp("bus-type", all_properties[1]) == 0 );
    assert( strcmp("vendor", all_properties[2]) == 0 );
    assert( strcmp("product", all_properties[3]) == 0 );
    assert( strcmp("name", all_properties[4]) == 0 );

    free(all_properties[0]);
    free(all_properties[1]);
    free(all_properties[2]);
    free(all_properties[3]);
    free(all_properties[4]);
    free(all_properties);

    /* Check multiple valued properties */
    printf("Checking properties with multiple values...\n");
    all_values = osi_get_device_property_all_values(device, "name", &len, &err);
    assert ( err == 0 );
    assert ( len == 2 );
    assert ( all_values != NULL );
    assert ( strcmp("ES1370", all_values[0]) == 0 );
    assert ( strcmp("es1370", all_values[1]) == 0 );
    free(all_values[0]);
    free(all_values[1]);
    free(all_values);

    value = osi_get_device_property_value(device, "name", &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp("ES1370", value) == 0 );
    free(value);

    /* Check single valued property */
    printf("Checking single valued property...\n");
    value = osi_get_device_property_value(device, "bus-type", &err);
    assert ( err == 0 );
    assert ( value != NULL );
    assert ( strcmp("pci", value) == 0 );
    free(value);

    all_values = osi_get_device_property_all_values(device, "bus-type", &len, &err);
    assert ( err == 0 );
    assert ( len == 1 );
    assert ( all_values != NULL );
    assert ( strcmp("pci", all_values[0]) == 0 );
    free(all_values[0]);
    free(all_values);

    /* Test device driver */
    printf("Test device drivers...\n");
    f10_driver = osi_get_device_driver(device, "audio", f10, &err);
    assert ( err == 0 );
    assert ( f10_driver != NULL);
    assert ( strcmp(f10_driver, "ac97") == 0 );
    free(f10_driver);

    f11_driver = osi_get_device_driver(device, "audio", f11, &err);
    assert ( err == 0 );
    assert ( f11_driver != NULL);
    assert ( strcmp(f11_driver, "ac97+") == 0 );
    free(f11_driver);

    /* Dispose of device and os handles */
    ret = osi_cleanup_handle(device);
    assert ( ret == 0 );
    ret = osi_cleanup_handle(f10);
    assert ( ret == 0 );
    ret = osi_cleanup_handle(f11);
    assert ( ret == 0 );

    /* Close library */
    printf("Closing library...\n");
    ret = osi_close_lib(lib);
    assert ( ret == 0 );

    printf("test-manipulate_devices succeeded.\n");
    return 0;
}