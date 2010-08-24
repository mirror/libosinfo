#include <osinfo/osinfo.h>

void __osinfoFreeDeviceLink(gpointer ptr)
{
    if (!ptr)
        return;
    struct __osinfoDeviceLink *devLink = (struct __osinfoDeviceLink *) ptr;
    g_free(devLink->driver);
    g_free(devLink);
}

void __osinfoFreeDeviceSection(gpointer tree)
{
    if (!tree)
        return;
    g_tree_destroy((GTree *)tree);
}

gint __osinfoStringCompare(gconstpointer a,
                           gconstpointer b,
                           gpointer data)
{
    // a and b are each gchar *, data is ignored
    gchar *str1 = (gchar *) a;
    gchar *str2 = (gchar *) b;
    return g_strcmp0(str1, str2);
}

gint __osinfoStringCompareBase(gconstpointer a,
                               gconstpointer b)
{
    // a and b are each gchar *, data is ignored
    gchar *str1 = (gchar *) a;
    gchar *str2 = (gchar *) b;
    return g_strcmp0(str1, str2);
}

gint __osinfoIntCompareBase(gconstpointer a,
                            gconstpointer b)
{
    // a and b are each gchar *, data is ignored
    unsigned long int1 = (unsigned long) a;
    unsigned long int2 = (unsigned long) b;
    return a - b;
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

void __osinfoFreeRelationship(gpointer ptrarray)
{
    if (!ptrarray)
        return;
    __osinfoFreePtrArray(ptrarray);
}

void __osinfoFreeParamVals(gpointer ptrarray)
{
    if (!ptrarray)
        return;
    __osinfoFreePtrArray(ptrarray);
}

void __osinfoFreeOsLink(gpointer ptr)
{
    if (!ptr)
        return;
    struct __osinfoOsLink *osLink = (struct __osinfoOsLink *) ptr;
    g_free(osLink);
}
