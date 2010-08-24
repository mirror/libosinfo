#include <osinfo/osinfo.h>

gint __osinfoStringCompare(gconstpointer a,
                           gconstpointer b,
                           gpointer data)
{
    // a and b are each gchar *, data is ignored
    gchar *str1 = (gchar *) a;
    gchar *str2 = (gchar *) b;
    return g_strcmp0(str1, str2);
}


gint __osinfoIntCompare(gconstpointer a,
                        gconstpointer b,
                        gpointer data)
{
    // a and b are each gchar *, data is ignored
    unsigned long int1 = (unsigned long) a;
    unsigned long int2 = (unsigned long) b;
    return a - b;
}


void __osinfoFreePtrArray(gpointer ptrarray)
{
    g_ptr_array_free(ptrarray, TRUE);
}
