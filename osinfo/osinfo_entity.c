#include <osinfo/osinfo.h>

G_DEFINE_ABSTRACT_TYPE (OsinfoEntity, osinfo_entity, G_TYPE_OBJECT);

#define OSINFO_ENTITY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_ENTITY, OsinfoEntityPrivate))

static void osinfo_entity_finalize (GObject *object);

enum OSI_ENTITY_PROPERTIES {
    OSI_ENTITY_PROP_0,

    OSI_ENTITY_ID,
};

static void
osinfo_entity_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    OsinfoEntity *self = OSINFO_ENTITY (object);

    switch (property_id)
      {
      case OSI_ENTITY_ID:
        g_free(self->priv->id);
        self->priv->id = g_value_dup_string (value);
        break;
      default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
      }
}

static void
osinfo_entity_get_property (GObject    *object,
                            guint       property_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
    OsinfoEntity *self = OSINFO_ENTITY (object);

    switch (property_id)
      {
      case OSI_ENTITY_ID:
        g_value_set_string (value, self->priv->id);
        break;
      default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
      }
}

static void
osinfo_entity_finalize (GObject *object)
{
    OsinfoEntity *self = OSINFO_ENTITY (object);

    g_free (self->priv->id);
    g_tree_destroy (self->priv->params);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_entity_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_entity_class_init (OsinfoEntityClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);
    GParamSpec *pspec;

    g_klass->set_property = osinfo_entity_set_property;
    g_klass->get_property = osinfo_entity_get_property;

    pspec = g_param_spec_string ("id",
                                 "ID",
                                 "Contains unique identifier for entity.",
                                 NULL /* default value */,
                                 G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
    g_object_class_install_property (g_klass,
                                     OSI_ENTITY_ID,
                                     pspec);

    g_klass->finalize = osinfo_entity_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoEntityPrivate));
}

static void
osinfo_entity_init (OsinfoEntity *self)
{
    OsinfoEntityPrivate *priv;
    self->priv = priv = OSINFO_ENTITY_GET_PRIVATE(self);

    self->priv->params = g_tree_new_full(__osinfoStringCompare, NULL, g_free, __osinfoFreeParamVals);
}

int osinfo_entity_add_param(OsinfoEntity *self, gchar *key, gchar *value)
{
    if (!OSINFO_IS_ENTITY(self) || !key || !value)
        return -EINVAL;

    // First check if there exists an existing array of entries for this key
    // If not, create a ptrarray of strings for this key and insert into map
    gboolean found;
    gpointer origKey, foundValue;
    GPtrArray *valueArray;
    gchar *valueDup = NULL, *keyDup = NULL;

    valueDup = g_strdup(value);

    found = g_tree_lookup_extended(self->priv->params, key, &origKey, &foundValue);
    if (!found) {
        keyDup = g_strdup(key);
        valueArray = g_ptr_array_new_with_free_func(g_free);

        g_tree_insert(self->priv->params, keyDup, valueArray);
    }
    else
        valueArray = (GPtrArray *) foundValue;

    // Add a copy of the value to the array
    g_ptr_array_add(valueArray, valueDup);
    return 0;
}

void osinfo_entity_clear_param(OsinfoEntity *self, gchar *key)
{
    g_tree_remove(self->priv->params, key);
}

gboolean osinfo_get_keys(gpointer key, gpointer value, gpointer data)
{
    GPtrArray *results = data;
    gchar *keyDup = g_strdup(key);

    g_ptr_array_add(results, keyDup);
    return FALSE; // Continue iterating
}

void osinfo_dup_array(gpointer data, gpointer user_data)
{
    GPtrArray *results = data;
    gchar *valueDup = g_strdup(data);

    g_ptr_array_add(results, valueDup);
}

gchar *osinfo_entity_get_id(OsinfoEntity *self)
{
    g_return_val_if_fail(OSINFO_IS_ENTITY(self), NULL);

    gchar *dupId = g_strdup(self->priv->id);

    return dupId;
}

GPtrArray *osinfo_entity_get_params(OsinfoEntity *self)
{
    g_return_val_if_fail(OSINFO_IS_ENTITY(self), NULL);

    GPtrArray *params = g_ptr_array_new();

    g_tree_foreach(self->priv->params, osinfo_get_keys, params);

    return params;
}

gchar *osinfo_entity_get_param_value(OsinfoEntity *self, gchar *key)
{
    g_return_val_if_fail(OSINFO_IS_ENTITY(self), NULL);
    g_return_val_if_fail(key != NULL, NULL);

    gboolean found;
    gpointer origKey, value;
    gchar *firstValueDup;
    GPtrArray *array;

    found = g_tree_lookup_extended(self->priv->params, key, &origKey, &value);
    if (!found)
        return NULL;
    array = (GPtrArray *) value;
    if (array->len == 0)
        return NULL;

    firstValueDup = g_strdup(g_ptr_array_index(array, 0));

    return firstValueDup;
}

GPtrArray *osinfo_entity_get_param_all_values(OsinfoEntity *self, gchar *key)
{
    g_return_val_if_fail(OSINFO_IS_ENTITY(self), NULL);
    g_return_val_if_fail(key != NULL, NULL);

    gboolean found;
    gpointer origKey, value;
    GPtrArray *srcArray, *retArray;

    retArray = g_ptr_array_new();

    found = g_tree_lookup_extended(self->priv->params, key, &origKey, &value);
    if (!found)
        return retArray;
    srcArray = (GPtrArray *) value;
    if (srcArray->len == 0)
        return retArray;

    g_ptr_array_foreach(srcArray, osinfo_dup_array, retArray);

    return retArray;
}
