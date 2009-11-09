#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#define KVM "http://qemu.org/qemu-kvm-0.11.0"
#define XEN "http://bits.xensource.com/oss-xen/release/3.4.1"

int main(void)
{
    int ret, num;
    char** hypervisor_rdfs;
    char* hv_one_rdf;
    char* hv_two_rdf;
    osi_lib_t lib_hv_one, lib_hv_two, lib_hv_default;
    osi_hypervisor_t hv_one, hv_two, hv_default;
    
    printf("Starting test-hypervisor...\n");
    printf("Get library handles...\n");
    lib_hv_one = osi_get_lib_handle();
    ret = osi_is_error(lib_hv_one);
    assert ( ret == 0 );
    
    lib_hv_two = osi_get_lib_handle();
    ret = osi_is_error(lib_hv_two);
    assert ( ret == 0 );
    
    lib_hv_default = osi_get_lib_handle();
    ret = osi_is_error(lib_hv_default);
    assert ( ret == 0 );
    
    /* Get a list of the hypervisor rdfs */
    printf("Getting list of hypervisor rdfs...\n");
    hypervisor_rdfs = osi_get_all_hypervisor_rdfs(lib_hv_one, &num);
    assert ( num == 2 ); /* Only two defined in the dummy data */
    assert ( hypervisor_rdfs != NULL );
    ret = (strcmp(KVM, hypervisor_rdfs[0]) == 0) || 
          (strcmp(XEN, hypervisor_rdfs[0] == 0);
    assert ( ret == 0 );
    if (strcmp(KVM, hypervisor_rdfs[0]) == 0)
        assert ( strcmp (XEN, hypervisor_rdfs[1]) == 0 );
    else
        assert ( strcmp (KVM, hypervisor_rdfs[1]) == 0);
    
    /* Check bad inputs to setting hypervisors */
    printf("Check that setting hypervisor fails for bad input...\n");
    ret = osi_set_hypervisor(lib_hv_one, "bad_param");
    assert ( ret == -EINVAL );
    ret = osi_set_hypervisor(lib_hv_two, "another_bad_param");
    assert ( ret == -EINVAL );    
    
    /* Set hypervisors for libraries */
    printf("Setting hypervisors for library instances...\n");
    ret = osi_set_hypervisor(lib_hv_one, hypervisor_rdfs[0]);
    assert ( ret == 0 );
    ret = osi_set_hypervisor(lib_hv_two, hypervisor_rdfs[1]);
    assert ( ret == 0 );
    
    /* Get handles to the hypervisors for each library instance */
    printf("Check that the hypervisors were set correctly...\n");
    hv_one = osi_get_hypervisor(lib_hv_one);
    ret = osi_is_error(hv_one);
    assert ( ret == 0 );
    hv_one_rdf = osi_get_hv_id(hv_one);
    assert ( hv_one_rdf != NULL );
    assert ( strcmp(hypervisor_rdfs[0], hv_one_rdf) == 0 );
    free(hv_one_rdf);
    
    hv_two = osi_get_hypervisor(lib_hv_two);
    ret = osi_is_error(hv_two);
    assert ( ret == 0 );
    hv_two_rdf = osi_get_hv_id(hv_two);
    assert ( hv_two_rdf != NULL );
    assert ( strcmp(hypervisor_rdfs[1], hv_two_rdf) == 0 );
    free(hv_two_rdf);
    
    /* Check that the default hypervisor is set if we don't specify one */
    printf("Check that hypervisor set to default if no value given...\n");
    hv_default = osi_get_hypervisor(lib_hv_default);
    ret = osi_is_error(hv_default);
    assert ( ret == 0 );
    hv_default_rdf = osi_get_hv_id(hv_default);
    assert ( hv_default_rdf != NULL);
    assert ( strcmp(DEFAULT_HYPERVISOR, hv_default_rdf) == 0 );
    free(hv_default_rdf);
    
    /* Check that specifying NULL chooses the default HV */
    printf("Check that default hypervisor can be chosen...\n");
    ret = osi_set_hypervisor(lib_hv_one, NULL);
    assert ( ret == 0 );
    hv_one_rdf = osi_get_hv_id(hv_one);
    assert ( hv_one_rdf != NULL );
    assert ( strcmp(DEFAULT_HYPERVISOR, hv_one_rdf);
    free(hv_default_rdf);
    
    /* And that we can, yet again, switch back */
    printf("Test switching hypervisors yet again...\n");
    ret = osi_set_hypervisor(lib_hv_one, hypervisor_rdfs[0]);
    assert ( ret == 0 );
    hv_one_rdf = osi_get_hv_id(hv_one);
    assert ( hv_one_rdf != NULL );
    assert ( strcmp(hypervisor_rdfs[0], hv_one_rdf) == 0 );
    free(hv_one_rdf);
    
    /* Initializing library */
    printf("Initializing libraries...\n");
    ret = osi_init_lib(lib_hv_one);
    assert ( ret == 0 );
    ret = osi_init_lib(lib_hv_two);
    assert ( ret == 0 );
    ret = osi_init_lib(lib_hv_default);
    assert ( ret == 0 );
    
    /* Verify that we cannot change the HVs now */
    printf("Checking that hv is fixed after library init...\n");
    ret = osi_set_hypervisor(lib_hv_one, hypervisor_rdfs[0]);
    assert ( ret == -EBUSY );
    ret = osi_set_hypervisor(lib_hv_two, hypervisor_rdfs[1]);
    assert ( ret == -EBUSY );
    ret = osi_set_hypervisor(lib_hv_default, NULL);
    assert ( ret == -EBUSY );
    
    /* Verify that hv hasn't changed as a result of the above attempt */
    hv_one_rdf = osi_get_hv_id(hv_one);
    assert ( hv_one_rdf != NULL );
    assert ( strcmp(hypervisor_rdfs[0], hv_one_rdf) == 0 );
    free(hv_one_rdf);
    
    hv_two_rdf = osi_get_hv_id(hv_two);
    assert ( hv_two_rdf != NULL );
    assert ( strcmp(hypervisor_rdfs[1], hv_two_rdf) == 0 );
    free(hv_two_rdf);

    hv_default_rdf = osi_get_hv_id(hv_default);
    assert ( hv_default_rdf != NULL );
    assert ( strcmp(DEFAULT_HYPERVISOR, hv_default_rdf) == 0 );
    free(hv_default_rdf);
    
    /* Close library */
    printf("Closing libraries...\n");
    ret = osi_close_lib(osi_lib_t lib_hv_one);
    assert ( ret == 0 );
    ret = osi_close_lib(osi_lib_t lib_hv_two);
    assert ( ret == 0 );
    ret = osi_close_lib(osi_lib_t lib_hv_default);
    assert ( ret == 0 );
    
    /* Verify handles are now invalid */
    printf("Verifying lib handles and hv handles are now invalid...\n");
    ret = osi_is_error(lib_hv_one);
    assert ( ret == 1 );
    ret = osi_is_error(lib_hv_two);
    assert ( ret == 1 );
    ret = osi_is_error(lib_hv_default);
    assert ( ret == 1 );
    ret = osi_is_error(hv_one);
    assert ( ret == 1 );
    ret = osi_is_error(hv_two);
    assert ( ret == 1 );
    ret = osi_is_error(hv_default);
    assert ( ret == 1 );
    
    free(hypervisor_rdfs);
    printf("test-hypervisor succeeded.\n");
    return 0;
}