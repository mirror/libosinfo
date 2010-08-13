#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoDb, osinfo_db, G_TYPE_OBJECT);

#define OSINFO_DB_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_DB, OsinfoDbPrivate))

struct _OsinfoDbPrivate
{
    int ready;

    gchar *backing_dir;
    gchar *libvirt_ver;

    OsinfoDeviceList *devices;
    OsinfoHypervisorList *hypervisors;
    OsinfoOsList *oses;
};


static void osinfo_db_set_property(GObject * object, guint prop_id,
                                         const GValue * value,
                                         GParamSpec * pspec);
static void osinfo_db_get_property(GObject * object, guint prop_id,
                                         GValue * value,
                                         GParamSpec * pspec);
static void osinfo_db_finalize (GObject *object);

enum OSI_DB_PROPERTIES {
    OSI_DB_PROP_0,

    OSI_DB_BACKING_DIR,
    OSI_DB_LIBVIRT_VER,
};

static void
osinfo_db_finalize (GObject *object)
{
    OsinfoDb *self = OSINFO_DB (object);

    g_free (self->priv->backing_dir);
    g_free (self->priv->libvirt_ver);

    g_object_unref(self->priv->devices);
    g_object_unref(self->priv->hypervisors);
    g_object_unref(self->priv->oses);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_db_parent_class)->finalize (object);
}

static void
osinfo_db_set_property (GObject      *object,
                        guint         property_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
    OsinfoDb *self = OSINFO_DB (object);

    switch (property_id)
      {
      case OSI_DB_BACKING_DIR:
        g_free(self->priv->backing_dir);
        self->priv->backing_dir = g_value_dup_string (value);
        break;

      case OSI_DB_LIBVIRT_VER:
        g_free(self->priv->libvirt_ver);
        self->priv->libvirt_ver = g_value_dup_string (value);
        break;

      default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
      }
}

static void
osinfo_db_get_property (GObject    *object,
                        guint       property_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
    OsinfoDb *self = OSINFO_DB (object);

    switch (property_id)
      {
      case OSI_DB_BACKING_DIR:
        g_value_set_string (value, self->priv->backing_dir);
        break;

      case OSI_DB_LIBVIRT_VER:
        g_value_set_string (value, self->priv->libvirt_ver);
        break;

      default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
      }
}

/* Init functions */
static void
osinfo_db_class_init (OsinfoDbClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);
    GParamSpec *pspec;

    g_klass->set_property = osinfo_db_set_property;
    g_klass->get_property = osinfo_db_get_property;
    g_klass->finalize = osinfo_db_finalize;

    pspec = g_param_spec_string ("backing-dir",
                                 "Backing directory",
                                 "Contains backing data store.",
                                 NULL /* default value */,
                                 G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
    g_object_class_install_property (g_klass,
                                     OSI_DB_BACKING_DIR,
                                     pspec);

    pspec = g_param_spec_string ("libvirt-ver",
                                 "Libvirt version",
                                 "Libvirt version user is interested in",
                                 NULL /* default value */,
                                 G_PARAM_READWRITE);
    g_object_class_install_property (g_klass,
                                     OSI_DB_LIBVIRT_VER,
                                     pspec);

    g_type_class_add_private (klass, sizeof (OsinfoDbPrivate));
}


static void
osinfo_db_init (OsinfoDb *self)
{
    OsinfoDbPrivate *priv;
    self->priv = priv = OSINFO_DB_GET_PRIVATE(self);

    self->priv->devices = osinfo_devicelist_new();
    self->priv->hypervisors = osinfo_hypervisorlist_new();
    self->priv->oses = osinfo_oslist_new();

    self->priv->ready = 0;
}

/** PUBLIC METHODS */

OsinfoDb *osinfo_db_new(const gchar *backingDir)
{
  return g_object_new (OSINFO_TYPE_DB,
		       "backing-dir", backingDir,
		       NULL);
}


int osinfo_db_initialize(OsinfoDb *self, GError **err)
{
    int ret;
    // And now read in data.
    ret = __osinfoInitializeData(self);
    if (ret != 0) {
        self->priv->ready = 0;
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), ret, "Error during reading data");
    }
    else
        self->priv->ready = 1;
    return ret;
}

OsinfoHypervisor *osinfo_db_get_hypervisor(OsinfoDb *self, gchar *id)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);
    g_return_val_if_fail(id != NULL, NULL);

    return OSINFO_HYPERVISOR(osinfo_list_find_by_id(OSINFO_LIST(self->priv->hypervisors), id));
}

OsinfoDevice *osinfo_db_get_device(OsinfoDb *self, gchar *id)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);
    g_return_val_if_fail(id != NULL, NULL);

    return OSINFO_DEVICE(osinfo_list_find_by_id(OSINFO_LIST(self->priv->hypervisors), id));
}

OsinfoOs *osinfo_db_get_os(OsinfoDb *self, gchar *id)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);
    g_return_val_if_fail(id != NULL, NULL);

    return OSINFO_OS(osinfo_list_find_by_id(OSINFO_LIST(self->priv->oses), id));
}


OsinfoOsList *osinfo_db_get_os_list(OsinfoDb *self)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);

    return self->priv->oses;
}

OsinfoHypervisorList *osinfo_db_get_hypervisor_list(OsinfoDb *self)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);

    return self->priv->hypervisors;
}

OsinfoDeviceList *osinfo_db_get_device_list(OsinfoDb *self)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);

    return self->priv->devices;
}

static gboolean osinfo_db_get_property_values_in_entity(OsinfoList *list, OsinfoEntity *entity, gpointer data)
{
    struct __osinfoPopulateValuesArgs *args = data;
    GTree *values = args->values;
    gchar *property = args->property;
    GPtrArray *valueArray = NULL;

    valueArray = g_tree_lookup(entity->priv->params, property);
    if (valueArray)
        return FALSE; // No values here, skip

    int i;
    for (i = 0; i < valueArray->len; i++) {
        gchar *currValue = g_ptr_array_index(valueArray, i);
        void *test = g_tree_lookup(values, currValue);
        if (test)
            continue;
        gchar *dupValue = g_strdup(currValue);

        // Add to tree with dummy value
        g_tree_insert(values, dupValue, (gpointer) 1);
    }

    return FALSE; // Continue iterating
}

static gboolean __osinfoPutKeysInList(gpointer key, gpointer value, gpointer data)
{
    gchar *currValue = (gchar *) key;
    GPtrArray *valuesList = (GPtrArray *) data;

    g_ptr_array_add(valuesList, currValue);
    return FALSE; // keep iterating
}


static GPtrArray *osinfo_db_unique_values_for_property_in_entity(OsinfoList *entities, gchar *propName)
{
    GTree *values = g_tree_new(__osinfoStringCompareBase);

    struct __osinfoPopulateValuesArgs args = { values, propName};
    osinfo_list_foreach(entities, osinfo_db_get_property_values_in_entity, &args);

    // For each key in tree, add to gptrarray
    GPtrArray *valuesList = g_ptr_array_sized_new(g_tree_nnodes(values));

    g_tree_foreach(values, __osinfoPutKeysInList, valuesList);
    g_tree_destroy(values);
    return valuesList;
}

// Get me all unique values for property "vendor" among operating systems
GPtrArray *osinfo_db_unique_values_for_property_in_os(OsinfoDb *self, gchar *propName)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);
    g_return_val_if_fail(propName != NULL, NULL);

    return osinfo_db_unique_values_for_property_in_entity(OSINFO_LIST(self->priv->oses), propName);
}

// Get me all unique values for property "vendor" among hypervisors
GPtrArray *osinfo_db_unique_values_for_property_in_hv(OsinfoDb *self, gchar *propName)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);
    g_return_val_if_fail(propName != NULL, NULL);

    return osinfo_db_unique_values_for_property_in_entity(OSINFO_LIST(self->priv->hypervisors), propName);
}

// Get me all unique values for property "vendor" among devices
GPtrArray *osinfo_db_unique_values_for_property_in_dev(OsinfoDb *self, gchar *propName)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);
    g_return_val_if_fail(propName != NULL, NULL);

    return osinfo_db_unique_values_for_property_in_entity(OSINFO_LIST(self->priv->devices), propName);
}

static gboolean __osinfoAddOsIfRelationship(OsinfoList *list, OsinfoEntity *entity, gpointer data)
{
    struct __osinfoOsCheckRelationshipArgs *args = data;
    OsinfoOs *os = OSINFO_OS(entity);
    OsinfoList *newList = args->list;

    GPtrArray *relatedOses = NULL;
    relatedOses = g_tree_lookup(os->priv->relationshipsByType, (gpointer) args->relshp);
    if (relatedOses) {
        osinfo_list_add(newList, OSINFO_ENTITY (os));
    }

    return FALSE;
}

// Get me all OSes that 'upgrade' another OS (or whatever relationship is specified)
OsinfoOsList *osinfo_db_unique_values_for_os_relationship(OsinfoDb *self, osinfoRelationship relshp)
{
    g_return_val_if_fail(OSINFO_IS_DB(self), NULL);

    // Create list
    OsinfoOsList *newList = osinfo_oslist_new();

    struct __osinfoOsCheckRelationshipArgs args = {OSINFO_LIST (newList), relshp};

    osinfo_list_foreach(OSINFO_LIST(self->priv->oses), __osinfoAddOsIfRelationship, &args);

    return newList;
}
