#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoList, osinfo_list, G_TYPE_OBJECT);

#define OSINFO_LIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_LIST, OsinfoListPrivate))

static void osinfo_list_finalize (GObject *object);

struct _OsinfoListPrivate
{
    GPtrArray *array;
};

static void
osinfo_list_finalize (GObject *object)
{
    OsinfoList *self = OSINFO_LIST (object);

    g_ptr_array_free(self->priv->array, TRUE);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_list_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_list_class_init (OsinfoListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_list_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoListPrivate));
}

static void
osinfo_list_init (OsinfoList *self)
{
    OsinfoListPrivate *priv;
    self->priv = priv = OSINFO_LIST_GET_PRIVATE(self);

    self->priv->array = g_ptr_array_new();
}

gint osinfo_list_get_length(OsinfoList *self)
{
    return self->priv->array->len;
}

OsinfoEntity *osinfo_list_get_nth(OsinfoList *self, gint idx)
{
    return g_ptr_array_index(self->priv->array, idx);
}

void __osinfo_list_add(OsinfoList *self, OsinfoEntity *entity)
{
    g_ptr_array_add(self->priv->array, entity);
}

void __osinfo_list_filter(OsinfoList *src, OsinfoList *dst, OsinfoFilter *filter)
{
    int i, len;
    len = osinfo_list_get_length(src);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(src, i);
        if (__osinfoEntityPassesFilter(filter, entity))
            __osinfo_list_add(dst, entity);
    }
}

OsinfoList *osinfo_list_filter(OsinfoList *self, OsinfoFilter *filter, GError **err)
{
    if (!OSINFO_IS_LIST(self)) {
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), -EINVAL, OSINFO_OBJ_NOT_LIST);
        return NULL;
    }

    if (filter && !OSINFO_IS_FILTER(filter)) {
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), -EINVAL, OSINFO_OBJ_NOT_FILTER);
        return NULL;
    }

    // For each element in self, if passes filter, add to new list.
    OsinfoList *newList = g_object_new(OSINFO_TYPE_LIST, NULL);

    __osinfo_list_filter(self, newList, filter);
    return newList;
}

int __osinfo_list_intersect(OsinfoList *src1, OsinfoList *src2, OsinfoList *dst)
{
    int i, len;

    // Make set representation of otherList and newList
    GTree *otherSet = g_tree_new(__osinfoStringCompareBase);
    GTree *newSet = g_tree_new(__osinfoStringCompareBase);

    // Add all from otherList to otherSet
    len = osinfo_list_get_length(src2);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(src2, i);
        gchar *id = entity->priv->id;
        g_tree_insert(otherSet, id, entity);
    }

    // If other contains entity, and new list does not, add to new list
    len = osinfo_list_get_length(src1);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(src1, i);
        gchar *id = entity->priv->id;

        if (g_tree_lookup(otherSet, entity->priv->id) &&
            !g_tree_lookup(newSet, entity->priv->id)) {
            g_tree_insert(newSet, id, entity);
            __osinfo_list_add(dst, entity);
        }
    }

    g_tree_destroy(otherSet);
    g_tree_destroy(newSet);
    return 0;
}

OsinfoList *osinfo_list_intersect(OsinfoList *self, OsinfoList *otherList, GError **err)
{
    if (!OSINFO_IS_LIST(self) || !OSINFO_IS_LIST(otherList)) {
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), -EINVAL, OSINFO_OBJ_NOT_LIST);
        return NULL;
    }

    OsinfoList *newList = g_object_new(OSINFO_TYPE_LIST, NULL);
    int ret;

    ret = __osinfo_list_intersect(self, otherList, newList);
    if (ret != 0) {
        g_object_unref(newList);
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), ret, __osinfoErrorToString(ret));
        return NULL;
    }

    return newList;
}

int __osinfo_list_union(OsinfoList *src1, OsinfoList *src2, OsinfoList *dst)
{
    // Make set version of new list
    GTree *newSet = g_tree_new(__osinfoStringCompareBase);

    // Add all from other list to new list
    int i, len;
    len = osinfo_list_get_length(src2);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(src2, i);
        gchar *id = entity->priv->id;
        __osinfo_list_add(dst, entity);
        g_tree_insert(newSet, id, entity);
    }

    // Add remaining elements from this list to new list
    len = osinfo_list_get_length(src1);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(src1, i);
        gchar *id = entity->priv->id;
        // If new list does not contain element, add to new list
        if (!g_tree_lookup(newSet, id)) {
            __osinfo_list_add(dst, entity);
            g_tree_insert(newSet, id, entity);
        }
    }

    g_tree_destroy(newSet);
    return 0;
}

OsinfoList *osinfo_list_union(OsinfoList *self, OsinfoList *otherList, GError **err)
{
    if (!OSINFO_IS_LIST(self) || !OSINFO_IS_LIST(otherList)) {
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), -EINVAL, OSINFO_OBJ_NOT_LIST);
        return NULL;
    }

    OsinfoList *newList = g_object_new(OSINFO_TYPE_LIST, NULL);
    int ret;
    ret = __osinfo_list_union(self, otherList, newList);
    if (ret != 0) {
        g_object_unref(newList);
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), ret, __osinfoErrorToString(ret));
        return NULL;
    }

    return newList;
}
