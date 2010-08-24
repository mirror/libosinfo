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


/** ****************************************************************************
 *      Convenience methods
 ******************************************************************************/

gint __osinfoIntCompare(gconstpointer a,
                        gconstpointer b,
                        gpointer data);
gint __osinfoStringCompare(gconstpointer a,
                           gconstpointer b,
                           gpointer data);

void __osinfoFreePtrArray(gpointer ptrarray);

/** ****************************************************************************
 *      Private structures for objects
 ******************************************************************************/



#endif /* __OSINFO_OBJECTS_H__ */
