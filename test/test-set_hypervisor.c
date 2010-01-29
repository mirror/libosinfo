#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <libosinfo.h>

#define KVM "http://qemu.org/qemu-kvm-0.11.0"
#define XEN "http://bits.xensource.com/oss-xen/release/3.4.1"

int main(int argc, char** argv)
{
    int ret, num, err;
    char** hypervisor_rdfs;
    char * hv_one_rdf, * hv_two_rdf;
    osi_lib_t lib_hv_one, lib_hv_two, lib_hv_default;
    osi_hypervisor_t hv_one, hv_two, hv_test;
    char* data_dir;

    assert ( argc == 2 );
    data_dir = argv[1];

    printf("Starting test-hypervisor...\n");
    printf("Get library handles...\n");
    lib_hv_one = osi_get_lib_handle(&err, data_dir);
    assert ( err == 0 );

    lib_hv_two = osi_get_lib_handle(&err, data_dir);
    assert ( err == 0 );

    lib_hv_default = osi_get_lib_handle(&err, data_dir);
    assert ( err == 0 );

    /* Get a list of the hypervisor rdfs */
    printf("Getting list of hypervisor rdfs...\n");
    hypervisor_rdfs = osi_get_all_hypervisor_ids(lib_hv_one, &num, &err);
    assert ( err == 0 );
    assert ( num == 2 ); /* Only two defined in the dummy data */
    assert ( hypervisor_rdfs != NULL );
    ret = (strcmp(KVM, hypervisor_rdfs[0]) == 0) ||
          (strcmp(XEN, hypervisor_rdfs[0] == 0));
    assert ( ret == 1 );
    if (strcmp(KVM, hypervisor_rdfs[0]) == 0)
        assert ( strcmp (XEN, hypervisor_rdfs[1]) == 0 );
    else
        assert ( strcmp (KVM, hypervisor_rdfs[1]) == 0);

    /* Check bad inputs to setting hypervisors */
    printf("Check that setting hypervisor fails for bad input...\n");
    ret = osi_set_lib_hypervisor(lib_hv_one, "bad_param");
    assert ( ret == -EINVAL );
    ret = osi_set_lib_hypervisor(lib_hv_two, "another_bad_param");
    assert ( ret == -EINVAL );

    /* Set hypervisors for libraries */
    printf("Setting hypervisors for library instances...\n");
    ret = osi_set_lib_hypervisor(lib_hv_one, hypervisor_rdfs[0]);
    assert ( ret == 0 );
    ret = osi_set_lib_hypervisor(lib_hv_two, hypervisor_rdfs[1]);
    assert ( ret == 0 );

    /* Get handles to the hypervisors for each library instance */
    printf("Check that the hypervisors were set correctly...\n");
    hv_one = osi_get_lib_hypervisor(lib_hv_one, &err);
    assert ( err == 0 );
    hv_one_rdf = osi_get_hv_id(hv_one, &err);
    assert ( err == 0 );
    assert ( hv_one_rdf != NULL );
    assert ( strcmp(hypervisor_rdfs[0], hv_one_rdf) == 0 );
    free(hv_one_rdf);

    hv_two = osi_get_lib_hypervisor(lib_hv_two, &err);
    assert ( err == 0 );
    hv_two_rdf = osi_get_hv_id(hv_two, &err);
    assert ( err == 0 );
    assert ( hv_two_rdf != NULL );
    assert ( strcmp(hypervisor_rdfs[1], hv_two_rdf) == 0 );
    free(hv_two_rdf);

    /* Check that the default hypervisor is set if we don't specify one */
    printf("Check that hypervisor set to default if no value given...\n");
    hv_test = osi_get_lib_hypervisor(lib_hv_default, &err);
    assert ( err == 0 );
    assert ( hv_test == NULL );

    /* Check that specifying NULL chooses the default HV */
    printf("Check that default hypervisor can be chosen...\n");
    ret = osi_set_lib_hypervisor(lib_hv_one, NULL);
    assert ( ret == 0 );
    hv_test = osi_get_lib_hypervisor(lib_hv_one, &err);
    assert ( err == 0 );
    assert ( hv_test == NULL );

    /* And that we can, yet again, switch back */
    printf("Test switching hypervisors yet again...\n");
    ret = osi_set_lib_hypervisor(lib_hv_one, hypervisor_rdfs[0]);
    assert ( ret == 0 );
    hv_test = osi_get_lib_hypervisor(lib_hv_one, &err);
    assert ( err == 0 );
    assert ( hv_test != NULL );
    hv_one_rdf = osi_get_hv_id(hv_test, &err);
    assert ( err == 0 );
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
    ret = osi_set_lib_hypervisor(lib_hv_one, hypervisor_rdfs[0]);
    assert ( ret == -EBUSY );
    ret = osi_set_lib_hypervisor(lib_hv_two, hypervisor_rdfs[1]);
    assert ( ret == -EBUSY );
    ret = osi_set_lib_hypervisor(lib_hv_default, NULL);
    assert ( ret == -EBUSY );

    /* Verify that hv hasn't changed as a result of the above attempt */
    hv_one_rdf = osi_get_hv_id(hv_one, &err);
    assert ( err == 0 );
    assert ( hv_one_rdf != NULL );
    assert ( strcmp(hypervisor_rdfs[0], hv_one_rdf) == 0 );
    free(hv_one_rdf);

    hv_two_rdf = osi_get_hv_id(hv_two, &err);
    assert ( err == 0 );
    assert ( hv_two_rdf != NULL );
    assert ( strcmp(hypervisor_rdfs[1], hv_two_rdf) == 0 );
    free(hv_two_rdf);

    /* Put refs that we got */
    osi_cleanup_handle(hv_one);
    osi_cleanup_handle(hv_two);
    osi_cleanup_handle(hv_test);

    /* Close library */
    printf("Closing libraries...\n");
    ret = osi_close_lib(lib_hv_one);
    assert ( ret == 0 );
    ret = osi_close_lib(lib_hv_two);
    assert ( ret == 0 );
    ret = osi_close_lib(lib_hv_default);
    assert ( ret == 0 );

    free(hypervisor_rdfs[0]);
    free(hypervisor_rdfs[1]);
    free(hypervisor_rdfs);
    printf("test-hypervisor succeeded.\n");
    return 0;
}
