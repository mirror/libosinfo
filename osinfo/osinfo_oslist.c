#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoOsList, osinfo_oslist, OSINFO_TYPE_LIST);

#define OSINFO_OSLIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_OSLIST, OsinfoOsListPrivate))

static void osinfo_oslist_finalize (GObject *object);

struct _OsinfoOsListPrivate
{
    int tmp;
};

static void
osinfo_oslist_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_oslist_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_oslist_class_init (OsinfoOsListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_oslist_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoOsListPrivate));
}

static void
osinfo_oslist_init (OsinfoOsList *self)
{
    OsinfoOsListPrivate *priv;
    self->priv = priv = OSINFO_OSLIST_GET_PRIVATE(self);

}


OsinfoOsList *osinfo_oslist_new(void)
{
    return g_object_new(OSINFO_TYPE_OSLIST, NULL);
}

OsinfoOsList *osinfo_oslist_new_filtered(OsinfoOsList *source, OsinfoFilter *filter)
{
    OsinfoOsList *newList = osinfo_oslist_new();
    osinfo_list_add_filtered(OSINFO_LIST(newList),
			     OSINFO_LIST(source),
			     filter);
    return newList;
}

OsinfoOsList *osinfo_oslist_new_intersection(OsinfoOsList *sourceOne, OsinfoOsList *sourceTwo)
{
    OsinfoOsList *newList = osinfo_oslist_new();
    osinfo_list_add_intersection(OSINFO_LIST(newList),
				 OSINFO_LIST(sourceOne),
				 OSINFO_LIST(sourceTwo));
    return newList;
}

OsinfoOsList *osinfo_oslist_new_union(OsinfoOsList *sourceOne, OsinfoOsList *sourceTwo)
{
    OsinfoOsList *newList = osinfo_oslist_new();
    osinfo_list_add_union(OSINFO_LIST(newList),
			  OSINFO_LIST(sourceOne),
			  OSINFO_LIST(sourceTwo));
    return newList;
}

