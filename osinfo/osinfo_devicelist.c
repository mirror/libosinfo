#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoDeviceList, osinfo_devicelist, OSINFO_TYPE_LIST);

#define OSINFO_DEVICELIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_DEVICELIST, OsinfoDeviceListPrivate))

static void osinfo_devicelist_finalize (GObject *object);

struct _OsinfoDeviceListPrivate
{
    gboolean unused;
};

static void
osinfo_devicelist_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_devicelist_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_devicelist_class_init (OsinfoDeviceListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_devicelist_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoDeviceListPrivate));
}

static void
osinfo_devicelist_init (OsinfoDeviceList *self)
{
    OsinfoDeviceListPrivate *priv;
    self->priv = priv = OSINFO_DEVICELIST_GET_PRIVATE(self);

}

OsinfoDeviceList *osinfo_devicelist_new(void)
{
    return g_object_new(OSINFO_TYPE_DEVICELIST, NULL);
}

OsinfoDeviceList *osinfo_devicelist_new_filtered(OsinfoDeviceList *source, OsinfoFilter *filter)
{
    OsinfoDeviceList *newList = osinfo_devicelist_new();
    osinfo_list_add_filtered(OSINFO_LIST(newList),
			     OSINFO_LIST(source),
			     filter);
    return newList;
}

OsinfoDeviceList *osinfo_devicelist_new_intersection(OsinfoDeviceList *sourceOne, OsinfoDeviceList *sourceTwo)
{
    OsinfoDeviceList *newList = osinfo_devicelist_new();
    osinfo_list_add_intersection(OSINFO_LIST(newList),
				 OSINFO_LIST(sourceOne),
				 OSINFO_LIST(sourceTwo));
    return newList;
}

OsinfoDeviceList *osinfo_devicelist_new_union(OsinfoDeviceList *sourceOne, OsinfoDeviceList *sourceTwo)
{
    OsinfoDeviceList *newList = osinfo_devicelist_new();
    osinfo_list_add_union(OSINFO_LIST(newList),
			  OSINFO_LIST(sourceOne),
			  OSINFO_LIST(sourceTwo));
    return newList;
}

