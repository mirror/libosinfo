#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoFilter, osinfo_filter, G_TYPE_OBJECT);

#define OSINFO_FILTER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_FILTER, OsinfoFilterPrivate))

struct _OsinfoFilterPrivate
{
    // Key: Constraint name
    // Value: GList of constraint values
    GHashTable *propertyConstraints;

    // Key: relationship type
    // Value: GList of OsinfoOs *
    // Note: Only used when filtering OsinfoOs objects
    GHashTable *relationshipConstraints;
};


static void osinfo_filter_finalize (GObject *object);

static void
osinfo_filter_finalize (GObject *object)
{
    OsinfoFilter *self = OSINFO_FILTER (object);

    g_hash_table_unref(self->priv->propertyConstraints);
    g_hash_table_unref(self->priv->relationshipConstraints);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_filter_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_filter_class_init (OsinfoFilterClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_filter_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoFilterPrivate));
}


OsinfoFilter *osinfo_filter_new(void)
{
    return g_object_new(OSINFO_TYPE_FILTER, NULL);
}


static void
osinfo_filter_prop_constraint_free(gpointer value, gpointer opaque G_GNUC_UNUSED)
{
    g_free(value);
}

static void
osinfo_filter_prop_constraints_free(gpointer props)
{
    g_list_foreach(props, osinfo_filter_prop_constraint_free, NULL);
}


static void
osinfo_filter_relshp_constraint_free(gpointer value, gpointer opaque G_GNUC_UNUSED)
{
    g_object_unref(value);
}

static void
osinfo_filter_relshp_constraints_free(gpointer relshps)
{
    g_list_foreach(relshps, osinfo_filter_relshp_constraint_free, NULL);
}

static void
osinfo_filter_init (OsinfoFilter *self)
{
    OsinfoFilterPrivate *priv;
    priv = OSINFO_FILTER_GET_PRIVATE(self);
    self->priv = priv;

    self->priv->propertyConstraints =
        g_hash_table_new_full(g_str_hash,
			      g_str_equal,
			      g_free,
			      osinfo_filter_prop_constraints_free);


    self->priv->relationshipConstraints =
        g_hash_table_new_full(g_int_hash,
			      g_int_equal,
			      NULL,
			      osinfo_filter_relshp_constraints_free);
}


gint osinfo_filter_add_constraint(OsinfoFilter *self, gchar *propName, gchar *propVal)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(self), -1);
    g_return_val_if_fail(propName != NULL, -1);
    g_return_val_if_fail(propVal != NULL, -1);

    // First check if there exists an array of entries for this key
    // If not, create a ptrarray of strings for this key and insert into map
    gboolean found;
    gpointer origKey, foundValue;
    GList *values = NULL;

    found = g_hash_table_lookup_extended(self->priv->propertyConstraints, propName, &origKey, &foundValue);
    if (found) {
        values = foundValue;
        g_hash_table_steal(self->priv->propertyConstraints, propName);
    }
    values = g_list_prepend(values, g_strdup(propVal));
    g_hash_table_insert(self->priv->propertyConstraints, g_strdup(propName), values);

    return 0;
}

// Only applicable to OSes, ignored by other types of objects
gint osinfo_filter_add_relation_constraint(OsinfoFilter *self, OsinfoOsRelationship relshp, OsinfoOs *os)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(self), -1);
    g_return_val_if_fail(OSINFO_IS_OS(os), -1);

    // First check if there exists an array of entries for this key
    // If not, create a ptrarray of strings for this key and insert into map
    gboolean found;
    gpointer origKey, foundValue;
    GList *values = NULL;

    found = g_hash_table_lookup_extended(self->priv->relationshipConstraints, GINT_TO_POINTER(relshp), &origKey, &foundValue);
    if (found) {
        values = foundValue;
        g_hash_table_steal(self->priv->propertyConstraints, GINT_TO_POINTER(relshp));
    }
    g_object_ref(os);
    values = g_list_prepend(values, os);
    g_hash_table_insert(self->priv->propertyConstraints, GINT_TO_POINTER(relshp), values);

    return 0;
}

void osinfo_filter_clear_constraint(OsinfoFilter *self, gchar *propName)
{
    g_hash_table_remove(self->priv->propertyConstraints, propName);
}

void osinfo_filter_clear_relationship_constraint(OsinfoFilter *self, OsinfoOsRelationship relshp)
{
    g_hash_table_remove(self->priv->relationshipConstraints, (gpointer) relshp);
}

void osinfo_filter_clear_all_constraints(OsinfoFilter *self)
{
    g_hash_table_remove_all(self->priv->propertyConstraints);
    g_hash_table_remove_all(self->priv->relationshipConstraints);
}

// get keyset for constraints map
GList *osinfo_filter_get_constraint_keys(OsinfoFilter *self)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(self), NULL);

    return g_hash_table_get_keys(self->priv->propertyConstraints);
}

// get values for given key
GList *osinfo_filter_get_constraint_values(OsinfoFilter *self, gchar *propName)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(self), NULL);
    g_return_val_if_fail(propName != NULL, NULL);

    return g_hash_table_lookup(self->priv->propertyConstraints, propName);
}

// get oses for given relshp
OsinfoOsList *osinfo_filter_get_relationship_constraint_value(OsinfoFilter *self, OsinfoOsRelationship relshp)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(self), NULL);

    // Create our list
    OsinfoOsList *newList = g_object_new(OSINFO_TYPE_OSLIST, NULL);

    GPtrArray *relatedOses = NULL;
    relatedOses = g_hash_table_lookup(self->priv->relationshipConstraints, GINT_TO_POINTER(relshp));
    if (relatedOses) {
        int i, len;
        len = relatedOses->len;
        for (i = 0; i < len; i++) {
             OsinfoOs *os = g_ptr_array_index(relatedOses, i);
	     osinfo_list_add(OSINFO_LIST (newList), OSINFO_ENTITY (os));
        }
    }

    return newList;
}


struct osinfo_filter_match_args {
    OsinfoFilter *self;
    osinfo_filter_match_func matcher;
    gpointer data;
    gboolean matched;
};

static void osinfo_filter_match_iterator(gpointer key, gpointer value, gpointer data)
{
    struct osinfo_filter_match_args *args = data;

    if (!(args->matcher)(args->self, key, value, args->data))
        args->matched = FALSE;
}

gboolean osinfo_filter_matches_constraints(OsinfoFilter *self,
					   osinfo_filter_match_func matcher,
					   gpointer data)
{
    struct osinfo_filter_match_args args = { self, matcher, data, TRUE };
    g_hash_table_foreach(self->priv->propertyConstraints,
			 osinfo_filter_match_iterator,
			 &args);
    return args.matched;
}


struct osinfo_filter_match_relation_args {
    OsinfoFilter *self;
    osinfo_filter_match_relation_func matcher;
    gpointer data;
    gboolean matched;
};

static void osinfo_filter_match_relation_iterator(gpointer key, gpointer value, gpointer data)
{
    struct osinfo_filter_match_args *args = data;

    if (!(args->matcher)(args->self, key, value, args->data))
        args->matched = FALSE;
}

gboolean osinfo_filter_matches_relation_constraints(OsinfoFilter *self,
						    osinfo_filter_match_relation_func matcher,
						    gpointer data)
{
    struct osinfo_filter_match_relation_args args = { self, matcher, data, TRUE };
    g_hash_table_foreach(self->priv->propertyConstraints,
			 osinfo_filter_match_relation_iterator,
			 &args);
    return args.matched;
}
