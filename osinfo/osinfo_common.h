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

gboolean osinfo_get_keys(gpointer key, gpointer value, gpointer data);
void osinfo_dup_array(gpointer data, gpointer user_data);

/** ****************************************************************************
 *      Private structures for objects
 ******************************************************************************/

struct _OsinfoHypervisorPrivate
{
    // Value: List of device_link structs
    GList *deviceLinks;
};

struct _OsinfoOsPrivate
{
    // OS-Hypervisor specific information
    // Key: gchar* (hypervisor id)
    // Value: GList: Element Value: List of device_link structs
    GHashTable *hypervisors;

    // Value: List of device_link structs
    GList *deviceLinks;

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

int __osinfoAddOsRelationship (OsinfoOs *self, gchar *otherOsId, osinfoRelationship rel);
void __osinfoClearOsRelationships (OsinfoOs *self, gchar *otherOsId);

#endif /* __OSINFO_OBJECTS_H__ */
