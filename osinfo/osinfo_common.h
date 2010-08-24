/*
 * libosinfo
 *
 * osinfo_objects.h
 * Contains forward declarations of our main object types, and definitions
 * of our internal data structures not exposed in the API as libosinfo objects.
 */

#ifndef __OSINFO_OBJECTS_H__
#define __OSINFO_OBJECTS_H__

#include <stdio.h>
#include <stdlib.h>
#include <glib-object.h>
#include <errno.h>

typedef struct _OsinfoEntity     OsinfoEntity;
typedef struct _OsinfoDb         OsinfoDb;
typedef struct _OsinfoDevice     OsinfoDevice;
typedef struct _OsinfoHypervisor OsinfoHypervisor;
typedef struct _OsinfoOs         OsinfoOs;
typedef struct _OsinfoFilter     OsinfoFilter;
typedef struct _OsinfoList       OsinfoList;
typedef struct _OsinfoDeviceList       OsinfoDeviceList;
typedef struct _OsinfoHypervisorList       OsinfoHypervisorList;
typedef struct _OsinfoOsList       OsinfoOsList;

typedef enum OSI_RELATIONSHIP {
    DERIVES_FROM,
    UPGRADES,
    CLONES,
} osinfoRelationship;


/** ****************************************************************************
 * Internal data structures
 ******************************************************************************/

struct __osinfoDeviceLink {
    OsinfoDevice *dev;
    gchar *driver;
};

struct __osinfoHvSection {
    OsinfoHypervisor *hv;
    OsinfoOs *os;

    GTree *sections; // Mapping GString key (device type) to GTree of deviceLink structs
    GTree *sectionsAsList; // Mapping GString key (device type) to Array of deviceLink structs
};

struct __osinfoOsLink {
    /* <subject_os> 'verbs' <direct_object_os>
     * fedora11 upgrades fedora10
     * centos clones rhel
     * scientificlinux derives from rhel
     */
    OsinfoOs *subjectOs;
    osinfoRelationship verb;
    OsinfoOs *directObjectOs;
};

struct __osinfoOsCheckRelationshipArgs {
    OsinfoList *list;
    osinfoRelationship relshp;
};


/** ****************************************************************************
 *      Convenience methods
 ******************************************************************************/

gint __osinfoIntCompareBase(gconstpointer a,
                            gconstpointer b);
gint __osinfoIntCompare(gconstpointer a,
                        gconstpointer b,
                        gpointer data);
gint __osinfoStringCompareBase(gconstpointer a,
                               gconstpointer b);
gint __osinfoStringCompare(gconstpointer a,
                           gconstpointer b,
                           gpointer data);

void __osinfoFreePtrArray(gpointer ptrarray);
void __osinfoFreeRelationship(gpointer ptrarray);
void __osinfoFreeParamVals(gpointer ptrarray);
void __osinfoFreeDeviceSection(gpointer tree);
void __osinfoFreeDeviceLink(gpointer ptr);
void __osinfoFreeOsLink(gpointer ptr);

void __osinfoListAdd(OsinfoList *self, OsinfoEntity *entity);

int __osinfoAddDeviceToSection(GTree *allSections, GTree *allSectionsAsList, gchar *sectionName, gchar *id, gchar *driver);
void __osinfoClearDeviceSection(GTree *allSections, GTree *allSectionsAsList, gchar *section);

gboolean osinfo_get_keys(gpointer key, gpointer value, gpointer data);
void osinfo_dup_array(gpointer data, gpointer user_data);

/** ****************************************************************************
 *      Private structures for objects
 ******************************************************************************/

struct _OsinfoHypervisorPrivate
{
    // Key: gchar* (device type)
    // Value: Tree of device_link structs (multiple devices per type)
    GTree *sections;
    GTree *sectionsAsList; // Mapping GString key (device type) to Array of deviceLink structs
};

struct _OsinfoOsPrivate
{
    // OS-Hypervisor specific information
    // Key: gchar* (hypervisor id)
    // Value: __osinfoHvSection struct
    GHashTable *hypervisors;

    // Key: gchar* (device type)
    // Value: Tree of device_link structs (multiple devices per type)
    GTree *sections;
    GTree *sectionsAsList; // Mapping GString key (device type) to Array of deviceLink structs

    // OS-OS relationships
    // Key: gchar* (other os id)
    // Value: Array of os_link structs
    GTree *relationshipsByOs;
    // Key: relationship type
    // Value: Array of os_link structs
    GTree *relationshipsByType;
};

struct _OsinfoEntityPrivate
{
    gchar *id;

    // Key: gchar*
    // Value: Array of gchar* values for key (multiple values allowed)
    GTree *params;
};

/** ****************************************************************************
 *      Private Methods
 ******************************************************************************/

// Private
int __osinfoAddDeviceToSectionOs(OsinfoOs *self, gchar *section, gchar *id, gchar *driver);
void __osinfoClearDeviceSectionOs(OsinfoOs *self, gchar *section);

int __osinfoAddOsRelationship (OsinfoOs *self, gchar *otherOsId, osinfoRelationship rel);
void __osinfoClearOsRelationships (OsinfoOs *self, gchar *otherOsId);

struct __osinfoHvSection *__osinfoAddHypervisorSectionToOs(OsinfoOs *self, gchar *hvId);
void __osinfoRemoveHvSectionFromOs(OsinfoOs *self, gchar *hvId);

// Private
int __osinfoAddDeviceToSectionHv(OsinfoHypervisor *self, gchar *section, gchar *id, gchar *driver);
void __osinfoClearDeviceSectionHv(OsinfoHypervisor *self, gchar *section);

#endif /* __OSINFO_OBJECTS_H__ */
