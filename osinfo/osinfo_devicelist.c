#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoDeviceList, osinfo_devicelist, OSINFO_TYPE_LIST);

#define OSINFO_DEVICELIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_DEVICELIST, OsinfoDeviceListPrivate))

static void osinfo_devicelist_finalize (GObject *object);

struct _OsinfoDeviceListPrivate
{
    int tmp;
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

OsinfoDevice *osinfo_device_list_get_nth(OsinfoDeviceList *self, gint idx, GError **err)
{
    if (!OSINFO_IS_DEVICELIST(self)) {
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), -EINVAL, OSINFO_OBJ_NOT_DEVICELIST);
        return NULL;
    }

    OsinfoList *selfAsList = OSINFO_LIST (self);
    OsinfoEntity *entity = osinfo_list_get_nth(selfAsList, idx);
    return OSINFO_DEVICE (entity);
}

OsinfoDeviceList *osinfo_device_list_filter(OsinfoDeviceList *self, OsinfoFilter *filter, GError **err)
{
    if (!OSINFO_IS_DEVICELIST(self)) {
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), -EINVAL, OSINFO_OBJ_NOT_DEVICELIST);
        return NULL;
    }

    if (filter && !OSINFO_IS_FILTER(filter)) {
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), -EINVAL, OSINFO_OBJ_NOT_FILTER);
        return NULL;
    }

    // For each element in self, if passes filter, add to new list.
    OsinfoDeviceList *newList = g_object_new(OSINFO_TYPE_DEVICELIST, NULL);
    if (!newList) {
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), -ENOMEM, OSINFO_NO_MEM);
        return NULL;
    }

    __osinfo_list_filter(OSINFO_LIST (self), OSINFO_LIST (newList), filter);
    return newList;
}

OsinfoDeviceList *osinfo_device_list_intersect(OsinfoDeviceList *self, OsinfoDeviceList *otherList, GError **err)
{
    if (!OSINFO_IS_DEVICELIST(self) || !OSINFO_IS_DEVICELIST(otherList)) {
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), -EINVAL, OSINFO_OBJ_NOT_DEVICELIST);
        return NULL;
    }

    OsinfoDeviceList *newList = g_object_new(OSINFO_TYPE_DEVICELIST, NULL);
    if (!newList) {
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), -ENOMEM, OSINFO_NO_MEM);
        return NULL;
    }

    int ret;

    ret = __osinfo_list_intersect(self, otherList, newList);
    if (ret != 0) {
        g_object_unref(newList);
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), ret, __osinfoErrorToString(ret));
        return NULL;
    }

    return newList;
}

OsinfoDeviceList *osinfo_device_list_union(OsinfoDeviceList *self, OsinfoDeviceList *otherList, GError **err)
{
    if (!OSINFO_IS_DEVICELIST(self) || !OSINFO_IS_DEVICELIST(otherList)) {
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), -EINVAL, OSINFO_OBJ_NOT_DEVICELIST);
        return NULL;
    }

    OsinfoDeviceList *newList = g_object_new(OSINFO_TYPE_DEVICELIST, NULL);
    if (!newList) {
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), -ENOMEM, OSINFO_NO_MEM);
        return NULL;
    }

    int ret;
    ret = __osinfo_list_union(self, otherList, newList);
    if (ret != 0) {
        g_object_unref(newList);
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), ret, __osinfoErrorToString(ret));
        return NULL;
    }

    return newList;
}
