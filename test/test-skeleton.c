#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <libosinfo.h>

int main(int argc, char** argv)
{
    int ret, err;
    osi_lib_t lib;
    char* data_dir;

    assert ( argc == 2 );
    data_dir = argv[1];

    printf("Starting test...\n");
    printf("Checking to see if we got library handle...\n");
    lib = osi_get_lib_handle(&err, data_dir);
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
