#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

int main(void)
{
    int ret;
    osi_lib_t lib;
    
    printf("Starting test...\n");
    printf("Checking to see if we got library handle...\n");
    lib = osi_get_lib_handle();
    ret = osi_is_error(lib);
    assert ( ret == 0 );
    
    /* Initializing library */
    printf("Initializing library...\n");
    ret = osi_init_lib(lib);
    assert ( ret == 0 );

    /* Close library */
    printf("Closing library...\n");
    ret = osi_close_lib(osi_lib_t lib);
    assert ( ret == 0 );
    
    printf("Verifying lib handle is now invalid...\n");
    ret = osi_is_error(lib);
    assert ( ret == 1 );
    
    printf("test succeeded.\n");
    return 0;
}