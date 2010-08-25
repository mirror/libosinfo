/*
 * libosinfo
 *
 * osinfo_devicelist.h
 */

#include <glib-object.h>
#include <osinfo/osinfo_filter.h>
#include <osinfo/osinfo_list.h>

#ifndef __OSINFO_DEVICELIST_H__
#define __OSINFO_DEVICELIST_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_DEVICELIST                  (osinfo_devicelist_get_type ())
#define OSINFO_DEVICELIST(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_DEVICELIST, OsinfoDeviceList))
#define OSINFO_IS_DEVICELIST(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_DEVICELIST))
#define OSINFO_DEVICELIST_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_DEVICELIST, OsinfoDeviceListClass))
#define OSINFO_IS_DEVICELIST_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_DEVICELIST))
#define OSINFO_DEVICELIST_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_DEVICELIST, OsinfoDeviceListClass))

typedef struct _OsinfoDeviceList        OsinfoDeviceList;

typedef struct _OsinfoDeviceListClass   OsinfoDeviceListClass;

typedef struct _OsinfoDeviceListPrivate OsinfoDeviceListPrivate;

/* object */
struct _OsinfoDeviceList
{
    OsinfoList parent_instance;

    /* public */

    /* private */
    OsinfoDeviceListPrivate *priv;
};

/* class */
struct _OsinfoDeviceListClass
{
    OsinfoListClass parent_class;

    /* class members */
};

GType osinfo_devicelist_get_type(void);

OsinfoDeviceList *osinfo_devicelist_new(void);
OsinfoDeviceList *osinfo_devicelist_new_filtered(OsinfoDeviceList *source, OsinfoFilter *filter);
OsinfoDeviceList *osinfo_devicelist_new_intersection(OsinfoDeviceList *sourceOne, OsinfoDeviceList *sourceTwo);
OsinfoDeviceList *osinfo_devicelist_new_union(OsinfoDeviceList *sourceOne, OsinfoDeviceList *sourceTwo);

#endif /* __OSINFO_DEVICELIST_H__ */
