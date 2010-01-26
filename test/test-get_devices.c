#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <libosinfo.h>

int main(int argc, char** argv)
{
    int i, ret, num, err, len;
    osi_lib_t lib;
    osi_hypervisor_t hv;
    osi_device_list_t all_audio_devices, subset_audio_devices;
    osi_device_t audio_device;
    osi_os_t os;
    osi_filter_t filter;
    char* value;
    char* data_dir;

    assert ( argc == 2 );
    data_dir = argv[1];

    printf("Starting test-get_devices...\n");
    printf("Checking to see if we got library handle...\n");
    lib = osi_get_lib_handle(&err, data_dir);
    assert ( err == 0 );

    /* Set hypervisor for library */
    printf("Setting hypervisor...\n");
    ret = osi_set_lib_hypervisor(lib, "http://qemu.org/qemu-kvm-0.11.0");
    assert ( ret == 0 );

    /* Initializing library */
    printf("Initializing library...\n");
    ret = osi_init_lib(lib);
    assert ( ret == 0 );

    /* Get handle to hypervisor */
    printf("Getting handle to hypervisor...\n");
    hv = osi_get_lib_hypervisor(lib, &err);
    assert ( err == 0 );

    /* Get all devices in the audio section for the hypervisor */
    printf("Getting all devices in the audio section...\n");
    all_audio_devices = osi_hypervisor_devices(hv, "audio", NULL, &err);
    assert ( err == 0 );

    /* Check that we have the expected number of devices */
    printf("Checking number of audio devices...\n");
    len = osi_devices_list_length(all_audio_devices);
    assert ( len == 3 );

    /* Check each device */
    printf("Checking first (out of three) audio devices...\n");
    audio_device  = osi_get_device_by_index(all_audio_devices, 0, &err);
    assert ( err == 0 );
    value = osi_device_id(audio_device, &err);
    assert ( err == 0 );
    assert ( strcmp("http://pci-ids.ucw.cz/read/PC/1002/4382", value) == 0 );
    free(value);
    ret = osi_cleanup_handle(audio_device);
    assert ( ret == 0 );

    printf("Checking second (out of three) audio devices...\n");
    audio_device  = osi_get_device_by_index(all_audio_devices, 1, &err);
    assert ( err == 0 );
    value = osi_device_id(audio_device, &err);
    assert ( err == 0 );
    assert ( strcmp("http://pci-ids.ucw.cz/read/PC/1274/5000", value) == 0 );
    free(value);
    ret = osi_cleanup_handle(audio_device);
    assert ( ret == 0 );

    printf("Checking third (out of three) audio devices...\n");
    audio_device  = osi_get_device_by_index(all_audio_devices, 2, &err);
    assert ( osi_is_error(audio_device) == 0 );
    value = osi_device_id(audio_device, &err);
    assert ( err == 0 );
    assert ( strcmp("http://pci-ids.ucw.cz/read/PC/1274/1371/80864541", value) == 0 );
    free(value);
    ret = osi_cleanup_handle(audio_device);
    assert ( ret == 0 );

    /* Done with the devices list from hypervisor, get rid of it */
    printf("Done with audio devices list from hypervisor, cleaning it up...\n");
    ret = osi_free_devices_list(all_audio_devices);
    assert ( ret == 0 );

    /* Get an OS by id so we can check <os,hv> device properties */
    printf("Getting an os object to test more device methods...\n");
    os = osi_get_os_by_id(lib, "http://fedoraproject.org/fedora-11", &err);
    assert ( err == 0 );

    /* Get all audio devices for supported by the os, for the given hv */
    printf("Getting all supported audio devices for given <os, hv>...\n");
    all_audio_devices =  osi_os_devices(os, "audio", NULL, &err);
    assert ( err == 0 );

    /* Check each device */
    printf("Checking first (out of three) audio devices for <os,hv>...\n");
    audio_device  = osi_get_device_by_index(all_audio_devices, 0, &err);
    assert ( err == 0 );
    value = osi_device_id(audio_device, &err);
    assert ( err == 0 );
    assert ( strcmp("http://pci-ids.ucw.cz/read/PC/1274/5000", value) == 0 );
    free(value);
    ret = osi_cleanup_handle(audio_device);
    assert ( ret == 0 );

    printf("Checking second (out of three) audio devices for <os,hv>...\n");
    audio_device  = osi_get_device_by_index(all_audio_devices, 1, &err);
    assert ( err == 0 );
    value = osi_device_id(audio_device, &err);
    assert ( err == 0 );
    assert ( strcmp("http://pci-ids.ucw.cz/read/PC/1002/4382", value) == 0 );
    free(value);
    ret = osi_cleanup_handle(audio_device);
    assert ( ret == 0 );

    printf("Checking third (out of three) audio devices for <os,hv>...\n");
    audio_device  = osi_get_device_by_index(all_audio_devices, 2, &err);
    assert ( err == 0 );
    value = osi_device_id(audio_device, &err);
    assert ( err == 0 );
    assert ( strcmp("http://pci-ids.ucw.cz/read/PC/1274/1371/80864541", value) == 0 );
    free(value);
    ret = osi_cleanup_handle(audio_device);
    assert ( ret == 0 );

    /* Done with the devices list from <os,hv>, get rid of it */
    printf("Done with audio devices list from <os,hv>, cleaning it up...\n");
    ret = osi_free_devices_list(all_audio_devices);
    assert ( ret == 0 );

    /* Set up a filter to get a reduced list of devices */
    filter = osi_get_filter(lib, &err);
    assert ( err == 0 );
    ret = osi_add_filter_constraint(filter, "vendor", "0x1274");
    assert ( ret == 0 );

    /* Now get all audio devices that match a given property/value */
    printf("Now getting all audio devices for <os,hv> for given property...\n");
    subset_audio_devices = osi_os_devices(os, "audio", filter, &err);
    assert ( err == 0 );

    /* Check each device */
    printf("Checking first (out of two) audio devices for <os,hv, property>...\n");
    audio_device  = osi_get_device_by_index(subset_audio_devices, 0, &err);
    assert ( err == 0 );
    value = osi_device_id(audio_device, &err);
    assert ( err == 0 );
    assert ( strcmp("http://pci-ids.ucw.cz/read/PC/1274/5000", value) == 0 );
    free(value);
    ret = osi_cleanup_handle(audio_device);
    assert ( ret == 0 );

    printf("Checking second (out of two) audio devices for <os,hv, property>...\n");
    audio_device  = osi_get_device_by_index(subset_audio_devices, 1, &err);
    assert ( err == 0 );
    value = osi_device_id(audio_device, &err);
    assert ( err == 0 );
    assert ( strcmp("http://pci-ids.ucw.cz/read/PC/1274/1371/80864541", value) == 0 );
    free(value);
    ret = osi_cleanup_handle(audio_device);
    assert ( ret == 0 );

    /* Done with the devices list from <os,hv, property>, get rid of it */
    printf("Done with audio devices list from <os,hv>, cleaning it up...\n");
    ret = osi_free_devices_list(subset_audio_devices);
    assert ( ret == 0 );

    /* Finally, check the preferred audio device for <os,hv> */
    audio_device = osi_get_preferred_device(os, "audio", filter, &err);
    assert ( err == 0 );
    value = osi_device_id(audio_device, &err);
    assert ( err == 0 );
    assert ( strcmp("http://pci-ids.ucw.cz/read/PC/1274/5000", value) == 0 );
    free(value);
    ret = osi_cleanup_handle(audio_device);
    assert ( ret == 0 );

    /* Get rid of filter */
    ret = osi_free_filter(filter);
    assert ( ret == 0 );

    /* Get rid of os handle */
    ret = osi_cleanup_handle(os);
    assert ( ret == 0 );

    /* Get rid of hv handle */
    ret = osi_cleanup_handle(hv);
    assert ( ret == 0 );

    /* Close library */
    printf("Closing library...\n");
    ret = osi_close_lib(lib);
    assert ( ret == 0 );

    printf("test-get_devices succeeded.\n");
    return 0;
}