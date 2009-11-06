#include <stdio.h>
#include "libosinfo.h"

int main(void)
{
  int i, ret, count;
  osi_lib_t lib = osi_get_lib_handle();

  ret = osi_set_lib_param(lib, "libvirt-version", "3.4");
  if (ret != 0) {
    printf("Error: Could not set libvirt version!\n");
    exit(1);
  }

  ret = osi_set_hypervisor(lib, "kvm", "1.2");
  if (ret != 0) {
    printf("Error: Could not set hypervisor!\n");
    exit(1);
  }

  ret = osi_init_lib(lib);
  if (ret != 0) {
    printf("Error: Could not set initialize libosinfo!\n");
    exit(1);
  }

  osi_filter_t filter = osi_get_filter(lib);
  ret = osi_add_constraint(filter, "short-id", "rhel5.4");
  if (ret != 0) {
    printf("Error: Could not set constraint!\n");
    exit(1);
  }

  osi_os_list_t results = osi_get_os_list(lib, filter);
  if (osi_bad_object(results))
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

  osi_os_t rhel = osi_get_os_by_index(results, 0);

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

  if (osi_add_constraint(filter, "kernel", "linux") != 0 ||
      osi_add_constraint(filter, "kernel-version", "2.6.30") != 0 ||
      osi_add_relation_constraint(filter, DERIVES_FROM, rhel) != 0)
  {
    printf("Error adding constraints!\n");
    exit(1);
  }

  osi_os_list_t more_results = osi_get_os_list(lib, filter);
  if (osi_bad_object(more_results))
    printf("Bad result list!\n");
    exit(1);
  }

  // For each os:
  count = osi_os_list_length(more_results);
  for (i = 0; i < count; i++) {
    int j, num;
    osi_os_t os = osi_os_by_index(more_results, i);
    char* osname = osi_get_property_pref_value(os, "name");

    osi_device_list_t audio_devices = osi_match_devices(os, "audio", "class", "audio");
    num = osi_devices_list_length(audio_devices);

    // For each audio device:
    for (j = 0; j < num; j++) {
      osi_device_t device = osi_get_device_by_index(audio_devices, j);
      printf("Audio device for %s:\n", distroname);
      printf("\tBus Type: %s Vendor: %s Product: %s\n", 
                 osi_get_device_property_value(device, "bus-type"),
                 osi_get_device_property_value(device, "vendor"),
                 osi_get_device_property_value(device, "product"));
      printf("\tDriver is: %s\n", osi_get_device_driver(device, distro));
    }

    // And free the distroname and list of audio devices for this distro
    free(distroname);
    ret = osi_put_devices_list(audio_devices);
    if (ret != 0) {
        printf("Error freeing devices list!\n");
        exit(1);
    }
  }

  ret = osi_put_distros_list(more_results); // Done with that list
  if (ret != 0) {
    printf("Error freeing distro list!\n");
    exit(1);
  }

  ret = osi_put_filter(filter); // Done with the filter
  if (ret != 0) {
    printf("Error freeing filter!\n"):
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