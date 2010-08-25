/*
 * libosinfo
 *
 * osinfo_hypervisorlist.h
 */

#include <glib-object.h>
#include <osinfo/osinfo_filter.h>
#include <osinfo/osinfo_list.h>

#ifndef __OSINFO_HYPERVISORLIST_H__
#define __OSINFO_HYPERVISORLIST_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_HYPERVISORLIST                  (osinfo_hypervisorlist_get_type ())
#define OSINFO_HYPERVISORLIST(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_HYPERVISORLIST, OsinfoHypervisorList))
#define OSINFO_IS_HYPERVISORLIST(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_HYPERVISORLIST))
#define OSINFO_HYPERVISORLIST_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_HYPERVISORLIST, OsinfoHypervisorListClass))
#define OSINFO_IS_HYPERVISORLIST_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_HYPERVISORLIST))
#define OSINFO_HYPERVISORLIST_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_HYPERVISORLIST, OsinfoHypervisorListClass))

typedef struct _OsinfoHypervisorList        OsinfoHypervisorList;

typedef struct _OsinfoHypervisorListClass   OsinfoHypervisorListClass;

typedef struct _OsinfoHypervisorListPrivate OsinfoHypervisorListPrivate;

/* object */
struct _OsinfoHypervisorList
{
    OsinfoList parent_instance;

    /* public */

    /* private */
    OsinfoHypervisorListPrivate *priv;
};

/* class */
struct _OsinfoHypervisorListClass
{
    OsinfoListClass parent_class;

    /* class members */
};

GType osinfo_hypervisorlist_get_type(void);

OsinfoHypervisorList *osinfo_hypervisorlist_new(void);
OsinfoHypervisorList *osinfo_hypervisorlist_new_filtered(OsinfoHypervisorList *source, OsinfoFilter *filter);
OsinfoHypervisorList *osinfo_hypervisorlist_new_intersection(OsinfoHypervisorList *sourceOne, OsinfoHypervisorList *sourceTwo);
OsinfoHypervisorList *osinfo_hypervisorlist_new_union(OsinfoHypervisorList *sourceOne, OsinfoHypervisorList *sourceTwo);

#endif /* __OSINFO_HYPERVISORLIST_H__ */
