#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <libosinfo.h>

int main(void)
{
  int i, ret, count, err;
  osi_lib_t lib;
  osi_filter_t filter;
  osi_os_t rhel, os;
  osi_os_list_t results, more_results;
  char* os_id;

  lib = osi_get_lib_handle(&err, "/home/arjun/Desktop/redhat/libosinfo/data");
  if (err != 0) {
      printf("Error: Could not get lib handle.\n");
      exit(1);
  }

  ret = osi_set_lib_param(lib, "libvirt-version", "3.4");
  if (ret != 0) {
    printf("Error: Could not set libvirt version!\n");
    exit(1);
  }

  ret = osi_set_lib_hypervisor(lib, "http://qemu.org/qemu-kvm-0.11.0");
  if (ret != 0) {
    printf("Error: Could not set hypervisor!\n");
    exit(1);
  }

  ret = osi_init_lib(lib);
  if (ret != 0) {
    printf("Error: Could not set initialize libosinfo!\n");
    exit(1);
  }

  filter = osi_get_filter(lib, &err);
  if (err != 0) {
      printf("Error: Could not get filter!\n");
      exit(1);
  }

  ret = osi_add_filter_constraint(filter, "short-id", "fedora11");
  if (ret != 0) {
    printf("Error: Could not set constraint!\n");
    exit(1);
  }

  results = osi_get_os_list(lib, filter, &err);
  if (err != 0) {
    printf("Bad result list!\n");
    exit(1);
  }

  if (osi_os_list_length(results) == 0) {
    printf("No results. Quitting...\n");
    exit(0);
  }
  else if (osi_os_list_length(results) > 1) {
    printf("Failed sanity check. 'short-id' should be unique...\n");
    exit(1);
  }

  rhel = osi_get_os_by_index(results, 0, &err);
  if (err != 0) {
    printf("Couldn't get os!\n");
    exit(1);
  }

  // Now that we have a handle to rhel5.4, we can free the results list
  // that we used to get to it. The handle to the single os is still
  // valid though, and we use it for the next step
  ret = osi_free_os_list(results); // Done with that list so get rid of it
  if (ret != 0) {
    printf("Error freeing os list!\n");
    exit(1);
  }

  // We shall reuse the filter
  ret = osi_clear_all_constraints(filter);
  if (ret != 0) {
    printf("Error clearing constraints!\n");
    exit(1);
  }

  if (osi_add_filter_constraint(filter, "vendor", "Fedora Project") != 0) {
    printf("Error adding constraints!\n");
    exit(1);
  }

  more_results = osi_get_os_list(lib, filter, &err);
  if (err != 0) {
    printf("Bad result list!\n");
    exit(1);
  }

  // For each os:
  count = osi_os_list_length(more_results);
  for (i = 0; i < count; i++) {
    int j, num;
    os = osi_get_os_by_index(more_results, i, &err);
    if (err != 0) {
        printf("Couldn't get os!\n");
        exit(1);
    }
    char* osname = osi_get_os_property_first_value(os, "name", &err);
    if (err != 0) {
        printf("Couldn't get property!\n");
        exit(1);
    }

    osi_device_list_t audio_devices = osi_os_devices(os, "audio", NULL, &err);
    num = osi_devices_list_length(audio_devices);
    os_id = osi_get_os_id(os, &err);

    // For each audio device:
    for (j = 0; j < num; j++) {
      osi_device_t device = osi_get_device_by_index(audio_devices, j, &err);
      printf("Audio device for %s:\n", os_id);
      printf("\tBus Type: %s Vendor: %s Product: %s\n", 
                 osi_get_device_property_value(device, "bus-type", &err),
                 osi_get_device_property_value(device, "vendor", &err),
                 osi_get_device_property_value(device, "product", &err));
      printf("\tDriver is: %s\n", osi_get_device_driver(device, "audio", os, &err));
    }

    // And free the os id string and list of audio devices for this distro
    free(os_id);
    ret = osi_free_devices_list(audio_devices);
    if (ret != 0) {
        printf("Error freeing devices list!\n");
        exit(1);
    }
  }

  ret = osi_free_os_list(more_results); // Done with that list
  if (ret != 0) {
    printf("Error freeing distro list!\n");
    exit(1);
  }

  ret = osi_free_filter(filter); // Done with the filter
  if (ret != 0) {
    printf("Error freeing filter!\n");
    exit(1);
  }

  ret = osi_close_lib(lib);
  if (ret != 0) {
    printf("Error cleaning up library handle!\n");
    exit(1);
  }

  printf("Done.\n");
  return 0;
}