#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoHypervisorList, osinfo_hypervisorlist, OSINFO_TYPE_LIST);

#define OSINFO_HYPERVISORLIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_HYPERVISORLIST, OsinfoHypervisorListPrivate))

static void osinfo_hypervisorlist_finalize (GObject *object);

struct _OsinfoHypervisorListPrivate
{
    gboolean unused;
};

static void
osinfo_hypervisorlist_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_hypervisorlist_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_hypervisorlist_class_init (OsinfoHypervisorListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_hypervisorlist_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoHypervisorListPrivate));
}

static void
osinfo_hypervisorlist_init (OsinfoHypervisorList *self)
{
    OsinfoHypervisorListPrivate *priv;
    self->priv = priv = OSINFO_HYPERVISORLIST_GET_PRIVATE(self);

}


OsinfoHypervisorList *osinfo_hypervisorlist_new(void)
{
    return g_object_new(OSINFO_TYPE_HYPERVISORLIST, NULL);
}

OsinfoHypervisorList *osinfo_hypervisorlist_new_filtered(OsinfoHypervisorList *source, OsinfoFilter *filter)
{
    OsinfoHypervisorList *newList = osinfo_hypervisorlist_new();
    osinfo_list_add_filtered(OSINFO_LIST(newList),
			     OSINFO_LIST(source),
			     filter);
    return newList;
}

OsinfoHypervisorList *osinfo_hypervisorlist_new_intersection(OsinfoHypervisorList *sourceOne, OsinfoHypervisorList *sourceTwo)
{
    OsinfoHypervisorList *newList = osinfo_hypervisorlist_new();
    osinfo_list_add_intersection(OSINFO_LIST(newList),
				 OSINFO_LIST(sourceOne),
				 OSINFO_LIST(sourceTwo));
    return newList;
}

OsinfoHypervisorList *osinfo_hypervisorlist_new_union(OsinfoHypervisorList *sourceOne, OsinfoHypervisorList *sourceTwo)
{
    OsinfoHypervisorList *newList = osinfo_hypervisorlist_new();
    osinfo_list_add_union(OSINFO_LIST(newList),
			  OSINFO_LIST(sourceOne),
			  OSINFO_LIST(sourceTwo));
    return newList;
}

