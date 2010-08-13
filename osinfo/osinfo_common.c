#include <osinfo/osinfo.h>

static int __osinfoAddDeviceToList(GTree *allSectionsAsList,
                                   gchar *sectionName,
                                   struct __osinfoDeviceLink *deviceLink)
{
    if (!allSectionsAsList || !sectionName || !deviceLink)
        return -EINVAL;

    gboolean found;
    gpointer origKey, foundValue;
    GPtrArray *sectionList;
    gchar *sectionNameDup = NULL;

    found = g_tree_lookup_extended(allSectionsAsList, sectionName, &origKey, &foundValue);
    if (!found) {
        sectionList = g_ptr_array_new();
        sectionNameDup = g_strdup(sectionName);
        g_tree_insert(allSectionsAsList, sectionNameDup, sectionList);
    }
    else
        sectionList = (GPtrArray *) foundValue;

    g_ptr_array_add(sectionList, deviceLink);
    return 0;
}

int __osinfoAddDeviceToSection(GTree *allSections, GTree *allSectionsAsList, gchar *sectionName, gchar *id, gchar *driver)
{
    if (!allSections || !sectionName || !id)
        return -EINVAL;

    gboolean found;
    gpointer origKey, foundValue;
    gchar *sectionNameDup = NULL, *idDup = NULL, *driverDup = NULL;
    GTree *section;
    struct __osinfoDeviceLink *deviceLink;
    int ret;

    idDup = g_strdup(id);
    driverDup = g_strdup(driver);
    deviceLink = g_new0(struct __osinfoDeviceLink, 1);

    found = g_tree_lookup_extended(allSections, sectionName, &origKey, &foundValue);
    if (!found) {
        section = g_tree_new_full(__osinfoStringCompare, NULL, g_free, __osinfoFreeDeviceLink);
        sectionNameDup = g_strdup(sectionName);

        g_tree_insert(allSections, sectionNameDup, section);
    }
    else
        section = (GTree *) foundValue;

    deviceLink->driver = driverDup;
    g_tree_insert(section, idDup, deviceLink);

    ret = 0;
    if (allSectionsAsList)
        ret = __osinfoAddDeviceToList(allSectionsAsList, sectionName, deviceLink);

    return ret;
}

void __osinfoClearDeviceSection(GTree *allSections, GTree *allSectionsAsList, gchar *section)
{
    if (!allSections || !section)
        return;

    g_tree_remove(allSections, section);
    g_tree_remove(allSectionsAsList, section);
}

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

void __osinfoFreeHvSection(gpointer ptr)
{
    if (!ptr)
        return;
    struct __osinfoHvSection * hvSection = (struct __osinfoHvSection *) ptr;
    g_tree_destroy(hvSection->sections);
    g_tree_destroy(hvSection->sectionsAsList);
    g_free(hvSection);
}
