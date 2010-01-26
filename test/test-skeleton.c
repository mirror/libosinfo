#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <libosinfo.h>

int main(void)
{
    int ret, err;
    osi_lib_t lib;

    printf("Starting test...\n");
    printf("Checking to see if we got library handle...\n");
    lib = osi_get_lib_handle(&err, "/home/arjun/Desktop/redhat/libosinfo/data");
    assert ( err == 0 );

    /* Initializing library */
    printf("Initializing library...\n");
    ret = osi_init_lib(lib);
    assert ( ret == 0 );

    /* Close library */
    printf("Closing library...\n");
    ret = osi_close_lib(lib);
    assert ( ret == 0 );

    printf("test succeeded.\n");
    return 0;
}
