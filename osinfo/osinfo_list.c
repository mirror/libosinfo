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

OsinfoEntity *osinfo_list_find_by_id(OsinfoList *self, const gchar *id)
{
    int i;
    for (i = 0 ; i < self->priv->array->len ; i++) {
        OsinfoEntity *ent = g_ptr_array_index(self->priv->array, i);
	const gchar *thisid = osinfo_entity_get_id(ent);
	if (g_strcmp0(id, thisid) == 0)
	    return ent;
    }
    return NULL;
}


void osinfo_list_add(OsinfoList *self, OsinfoEntity *entity)
{
    g_ptr_array_add(self->priv->array, entity);
}


void osinfo_list_add_filtered(OsinfoList *self, OsinfoList *source, OsinfoFilter *filter)
{
    int i, len;
    len = osinfo_list_get_length(source);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(source, i);
        if (__osinfoEntityPassesFilter(filter, entity))
	    osinfo_list_add(self, entity);
    }
}


void osinfo_list_add_intersection(OsinfoList *self, OsinfoList *sourceOne, OsinfoList *sourceTwo)
{
    int i, len;

    // Make set representation of otherList and newList
    GTree *otherSet = g_tree_new(__osinfoStringCompareBase);
    GTree *newSet = g_tree_new(__osinfoStringCompareBase);

    // Add all from otherList to otherSet
    len = osinfo_list_get_length(sourceTwo);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(sourceTwo, i);
        gchar *id = entity->priv->id;
        g_tree_insert(otherSet, id, entity);
    }

    // If other contains entity, and new list does not, add to new list
    len = osinfo_list_get_length(sourceOne);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(sourceOne, i);
        gchar *id = entity->priv->id;

        if (g_tree_lookup(otherSet, entity->priv->id) &&
            !g_tree_lookup(newSet, entity->priv->id)) {
            g_tree_insert(newSet, id, entity);
            osinfo_list_add(self, entity);
        }
    }

    g_tree_destroy(otherSet);
    g_tree_destroy(newSet);
}


void osinfo_list_add_union(OsinfoList *self, OsinfoList *sourceOne, OsinfoList *sourceTwo)
{
    // Make set version of new list
    GTree *newSet = g_tree_new(__osinfoStringCompareBase);

    // Add all from other list to new list
    int i, len;
    len = osinfo_list_get_length(sourceTwo);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(sourceTwo, i);
        gchar *id = entity->priv->id;
        osinfo_list_add(self, entity);
        g_tree_insert(newSet, id, entity);
    }

    // Add remaining elements from this list to new list
    len = osinfo_list_get_length(sourceOne);
    for (i = 0; i < len; i++) {
        OsinfoEntity *entity = osinfo_list_get_nth(sourceOne, i);
        gchar *id = entity->priv->id;
        // If new list does not contain element, add to new list
        if (!g_tree_lookup(newSet, id)) {
	    osinfo_list_add(self, entity);
            g_tree_insert(newSet, id, entity);
        }
    }

    g_tree_destroy(newSet);
}


void osinfo_list_foreach(OsinfoList *self, osinfo_list_iterator iter, gpointer data)
{
    int i;
    for (i = 0 ; i < self->priv->array->len ; i++) {
        OsinfoEntity *ent = g_ptr_array_index(self->priv->array, i);
	iter(self, ent, data);
    }
}
