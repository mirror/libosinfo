#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include <glib-object.h>
#include <osinfo.h>

int
main (int argc, char *argv[])
{
    int ret;
    g_type_init();

    /* Create our object */
    OsinfoDb *db = g_object_new (OSINFO_TYPE_DB, "backing-dir",
                                 "./data", NULL);

    // Read in data
    ret = osinfoInitializeDb(db, NULL);
    if (ret != 0) {
        printf("Error initializing db! %d\n", ret);
        exit(1);
    }

    gchar *backing_dir;
    gchar *libvirt_ver;

    g_object_get(G_OBJECT(db), "backing-dir", &backing_dir,
                               "libvirt-ver", &libvirt_ver,
                               NULL);


    printf("Backing dir : %s Libvirt ver: %s\n", backing_dir, libvirt_ver);
    g_free(backing_dir);
    g_free(libvirt_ver);

    // Now set libvirt-ver and test
    GValue val = { 0, };
    g_value_init (&val, G_TYPE_STRING);
    g_value_set_string(&val, "2.4");

    g_object_set_property (G_OBJECT (db), "libvirt-ver", &val);
    g_value_unset(&val);

    g_object_get(G_OBJECT(db), "backing-dir", &backing_dir,
                               "libvirt-ver", &libvirt_ver,
                               NULL);
    printf("Backing dir : %s Libvirt ver: %s\n", backing_dir, libvirt_ver);
    g_free(backing_dir);
    g_free(libvirt_ver);

    // Now try settomg backing-dir and test
    g_value_init (&val, G_TYPE_STRING);
    g_value_set_string(&val, "/evil/dir/");

    g_object_set_property (G_OBJECT (db), "backing-dir", &val);
    g_value_unset(&val);

    g_object_get(G_OBJECT(db), "backing-dir", &backing_dir,
                               "libvirt-ver", &libvirt_ver,
                               NULL);
    printf("Backing dir : %s Libvirt ver: %s\n", backing_dir, libvirt_ver);
    g_free(backing_dir);
    g_free(libvirt_ver);
    g_object_unref(db);
    return 0;
}
