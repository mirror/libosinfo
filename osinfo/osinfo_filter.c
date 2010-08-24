#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoFilter, osinfo_filter, G_TYPE_OBJECT);

#define OSINFO_FILTER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_FILTER, OsinfoFilterPrivate))

struct _OsinfoFilterPrivate
{
    // Key: Constraint name
    // Value: Array of constraint values
    GTree *propertyConstraints;

    // Key: relationship type
    // Value: Array of OsinfoOs *
    // Note: Only used when filtering OsinfoOs objects
    GTree *relationshipConstraints;
};

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


static void osinfo_filter_finalize (GObject *object);

static void
osinfo_filter_finalize (GObject *object)
{
    OsinfoFilter *self = OSINFO_FILTER (object);

    g_tree_destroy(self->priv->propertyConstraints);
    g_tree_destroy(self->priv->relationshipConstraints);

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

static void
osinfo_filter_init (OsinfoFilter *self)
{
    OsinfoFilterPrivate *priv;
    priv = OSINFO_FILTER_GET_PRIVATE(self);
    self->priv = priv;

    self->priv->propertyConstraints = g_tree_new_full(__osinfoStringCompare,
                                                     NULL,
                                                     g_free,
                                                     __osinfoFreePtrArray);


    self->priv->relationshipConstraints = g_tree_new_full(__osinfoIntCompare,
                                                         NULL,
                                                         NULL,
                                                         __osinfoFreePtrArray);
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
    GPtrArray *valueArray;
    gchar *valueDup = NULL, *keyDup = NULL;

    valueDup = g_strdup(propVal);

    found = g_tree_lookup_extended(self->priv->propertyConstraints, propName, &origKey, &foundValue);
    if (!found) {
        keyDup = g_strdup(propName);
        valueArray = g_ptr_array_new_with_free_func(g_free);

        g_tree_insert(self->priv->propertyConstraints, keyDup, valueArray);
    }
    else
        valueArray = (GPtrArray *) foundValue;

    // Add a copy of the value to the array
    g_ptr_array_add(valueArray, valueDup);
    return 0;
}

// Only applicable to OSes, ignored by other types of objects
gint osinfo_filter_add_relation_constraint(OsinfoFilter *self, osinfoRelationship relshp, OsinfoOs *os)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(self), -1);
    g_return_val_if_fail(OSINFO_IS_OS(os), -1);

    // First check if there exists an array of entries for this key
    // If not, create a ptrarray of strings for this key and insert into map
    gboolean found;
    gpointer origKey, foundValue;
    GPtrArray *valueArray;

    found = g_tree_lookup_extended(self->priv->relationshipConstraints, (gpointer) relshp, &origKey, &foundValue);
    if (!found) {
        valueArray = g_ptr_array_new();

        g_tree_insert(self->priv->relationshipConstraints, (gpointer) relshp, valueArray);
    }
    else
        valueArray = (GPtrArray *) foundValue;

    // Add to the array
    g_ptr_array_add(valueArray, os);
    return 0;
}

void osinfo_filter_clear_constraint(OsinfoFilter *self, gchar *propName)
{
    g_tree_remove(self->priv->propertyConstraints, propName);
}

void osinfo_filter_clear_relationship_constraint(OsinfoFilter *self, osinfoRelationship relshp)
{
    g_tree_remove(self->priv->relationshipConstraints, (gpointer) relshp);
}

static gboolean __osinfoRemoveTreeEntry(gpointer key, gpointer value, gpointer data)
{
    GTree *tree = (GTree *) data;
    g_tree_remove(tree, key);
    return FALSE; // continue iterating
}

void osinfo_filter_clear_all_constraints(OsinfoFilter *self)
{
    g_tree_foreach(self->priv->propertyConstraints, __osinfoRemoveTreeEntry, self->priv->propertyConstraints);
    g_tree_foreach(self->priv->relationshipConstraints, __osinfoRemoveTreeEntry, self->priv->relationshipConstraints);
}

// get keyset for constraints map
GPtrArray *osinfo_filter_get_constraint_keys(OsinfoFilter *self)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(self), NULL);

    GPtrArray *constraints = g_ptr_array_new();

    g_tree_foreach(self->priv->propertyConstraints, osinfo_get_keys, constraints);

    return constraints;
}

// get values for given key
GPtrArray *osinfo_filter_get_constraint_values(OsinfoFilter *self, gchar *propName)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(self), NULL);
    g_return_val_if_fail(propName != NULL, NULL);

    gboolean found;
    gpointer origKey, value;
    GPtrArray *srcArray, *retArray;

    retArray = g_ptr_array_new();

    found = g_tree_lookup_extended(self->priv->propertyConstraints, propName, &origKey, &value);
    if (!found)
        return retArray;
    srcArray = (GPtrArray *) value;
    if (srcArray->len == 0)
        return retArray;

    g_ptr_array_foreach(srcArray, osinfo_dup_array, retArray);

    return retArray;
}

// get oses for given relshp
OsinfoOsList *osinfo_filter_get_relationship_constraint_value(OsinfoFilter *self, osinfoRelationship relshp)
{
    g_return_val_if_fail(OSINFO_IS_FILTER(self), NULL);

    // Create our list
    OsinfoOsList *newList = g_object_new(OSINFO_TYPE_OSLIST, NULL);

    GPtrArray *relatedOses = NULL;
    relatedOses = g_tree_lookup(self->priv->relationshipConstraints, (gpointer) relshp);
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

static gboolean osinfo_filter_match_iterator(gpointer key, gpointer value, gpointer data)
{
    struct osinfo_filter_match_args *args = data;

    if (!(args->matcher)(args->self, key, value, args->data)) {
        args->matched = FALSE;
	return FALSE;
    }
    return TRUE;
}

gboolean osinfo_filter_matches_constraints(OsinfoFilter *self,
					   osinfo_filter_match_func matcher,
					   gpointer data)
{
    struct osinfo_filter_match_args args = { self, matcher, data, TRUE };
    g_tree_foreach(self->priv->propertyConstraints,
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

static gboolean osinfo_filter_match_relation_iterator(gpointer key, gpointer value, gpointer data)
{
    struct osinfo_filter_match_args *args = data;

    if (!(args->matcher)(args->self, key, value, args->data)) {
        args->matched = FALSE;
	return FALSE;
    }
    return TRUE;
}

gboolean osinfo_filter_matches_relation_constraints(OsinfoFilter *self,
						    osinfo_filter_match_relation_func matcher,
						    gpointer data)
{
    struct osinfo_filter_match_relation_args args = { self, matcher, data, TRUE };
    g_tree_foreach(self->priv->propertyConstraints,
		   osinfo_filter_match_relation_iterator,
		   &args);
    return args.matched;
}
