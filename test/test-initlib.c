#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

int main(void)
{
    int ret;
    char* lv_version;
    char* lv_recheck;
    char* bad_param;
    osi_lib_t lib;

    printf("Starting test-initlib...\n");
    printf("Checking to see if we got library handle...\n");
    lib = osi_get_lib_handle();
    ret = osi_is_error(lib);
    assert ( ret == 0 );
    
    /* Setting a parameter */
    printf("Setting parameter...\n");
    ret = osi_set_lib_param(lib, "libvirt-version", "0.6.2");
    assert ( ret == 0 );
    
    /* Getting a parameter that has been set */
    printf("Getting parameter...\n");
    lv_version = osi_get_lib_param(lib, "libvirt-version");
    assert ( lv_version != NULL );
    assert ( strcmp(lv_version, "0.6.2") == 0 );
    
    /* Getting parameter that has not been set */
    printf("Getting bad parameter...\n");
    bad_param = osi_get_lib_param(lib, "bad-param");
    assert ( bad_param == NULL );
    
    /* Initializing library */
    printf("Initializing library...\n");
    ret = osi_init_lib(lib);
    assert ( ret == 0 );

    /* Try setting already set parameter after init */
    printf("Try setting parameter after init...\n");
    ret = osi_set_lib_param(lib, "libvirt-version", "0.6.2");
    assert ( ret == -EBUSY );
    
    /* Try setting new parameter after init */
    printf("Try setting parameter after init...\n");
    ret = osi_set_lib_param(lib, "new-param", "new-value");
    assert ( ret == -EBUSY );
    
    /* Double check set parameter */
    printf("Rechecking parameter...\n");
    lv_recheck = osi_get_lib_param(lib, "libvirt-version");
    assert ( ret != NULL );
    assert ( strcmp(lv_recheck, "0.6.2") == 0 );
    assert ( strcmp(lv_recheck, lv_version) == 0 );
    free(lv_version);
    free(lv_recheck);
    
    /* Close library */
    printf("Closing library...\n");
    ret = osi_close_lib(osi_lib_t lib);
    assert ( ret == 0 );
    
    printf("Verifying lib handle is now invalid...\n");
    ret = osi_is_error(lib);
    assert ( ret == 1 );
    
    printf("test-initlib succeeded.\n");
    return 0;
}