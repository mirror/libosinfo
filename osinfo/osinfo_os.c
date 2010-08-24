#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoOs, osinfo_os, OSINFO_TYPE_ENTITY);

#define OSINFO_OS_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_OS, OsinfoOsPrivate))

struct _OsinfoOsPrivate
{
    // OS-Hypervisor specific information
    // Key: gchar* (hypervisor id)
    // Value: GList: Element Value: List of device_link structs
    GHashTable *hypervisors;

    // Value: List of device_link structs
    GList *deviceLinks;

    // Value: Array of os_link structs
    GList *osLinks;
};

struct _OsinfoOsOsLink {
    /* <self> 'verbs' <other_os>
     * fedora11 upgrades fedora10
     * centos clones rhel
     * scientificlinux derives from rhel
     */
    osinfoRelationship relshp;
    OsinfoOs *otherOs;
};

struct _OsinfoOsDeviceLink {
    OsinfoDevice *dev;
    gchar *driver;
};


static void osinfo_os_finalize (GObject *object);

static void osinfo_device_link_free(gpointer data, gpointer opaque G_GNUC_UNUSED)
{
    struct _OsinfoOsDeviceLink *link = data;
    g_object_unref(link->dev);
    g_free(link->driver);
    g_free(link);
}

static void osinfo_os_link_free(gpointer data, gpointer opaque G_GNUC_UNUSED)
{
    struct _OsinfoOsOsLink *link = data;
    g_object_unref(link->otherOs);
    g_free(link);
}


static void
osinfo_os_finalize (GObject *object)
{
    OsinfoOs *self = OSINFO_OS (object);

    g_list_foreach(self->priv->deviceLinks, osinfo_device_link_free, NULL);
    g_list_free(self->priv->deviceLinks);
    g_hash_table_unref(self->priv->hypervisors);

    g_list_foreach(self->priv->osLinks, osinfo_os_link_free, NULL);
    g_list_free(self->priv->osLinks);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_os_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_os_class_init (OsinfoOsClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_os_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoOsPrivate));
}

static void
osinfo_os_hypervisor_devices_free(gpointer opaque)
{
    GList *deviceLinks = opaque;

    g_list_foreach(deviceLinks, osinfo_device_link_free, NULL);
    g_list_free(deviceLinks);
}

static void
osinfo_os_init (OsinfoOs *self)
{
    OsinfoOsPrivate *priv;
    self->priv = priv = OSINFO_OS_GET_PRIVATE(self);

    self->priv->deviceLinks = NULL;
    self->priv->osLinks = NULL;
    self->priv->hypervisors = g_hash_table_new_full(g_str_hash,
						    g_str_equal,
						    g_free,
						    osinfo_os_hypervisor_devices_free);
}

OsinfoOs *osinfo_os_new(const gchar *id)
{
    return g_object_new(OSINFO_TYPE_OS,
			"id", id,
			NULL);
}


OsinfoDevice *osinfo_os_get_preferred_device(OsinfoOs *self, OsinfoHypervisor *hv, gchar *devType, OsinfoFilter *filter,
					     const gchar **driver)
{
    g_return_val_if_fail(OSINFO_IS_OS(self), NULL);
    g_return_val_if_fail(OSINFO_IS_HYPERVISOR(hv), NULL);
    g_return_val_if_fail(OSINFO_IS_FILTER(filter), NULL);
    g_return_val_if_fail(devType != NULL, NULL);
    // Check if device type info present for <os,hv>, else return NULL.

    GList *tmp;
    if (hv)
        tmp = g_hash_table_lookup(self->priv->hypervisors,
				  osinfo_entity_get_id(OSINFO_ENTITY(hv)));
    else
        tmp = self->priv->deviceLinks;

    // For each device in section list, apply filter. If filter passes, return device.
    while (tmp) {
        struct _OsinfoOsDeviceLink *link = tmp->data;

        if (osinfo_entity_matches_filter(OSINFO_ENTITY(link->dev), filter)) {
	    *driver = link->driver;
	    return link->dev;
	}
    }

    // If no devices pass filter, return NULL.
    *driver= NULL;
    return NULL;
}

OsinfoOsList *osinfo_os_get_related(OsinfoOs *self, osinfoRelationship relshp)
{
    g_return_val_if_fail(OSINFO_IS_OS(self), NULL);

    // Create our list
    OsinfoOsList *newList = osinfo_oslist_new();
    GList *tmp = self->priv->osLinks;

    while (tmp) {
        struct _OsinfoOsOsLink *link = tmp->data;

	if (link->relshp == relshp)
	    osinfo_list_add(OSINFO_LIST(newList), OSINFO_ENTITY(link->otherOs));

        tmp = tmp->next;
    }

    return newList;
}

OsinfoDeviceList *osinfo_os_get_devices(OsinfoOs *self, OsinfoHypervisor *hv, OsinfoFilter *filter)
{
    g_return_val_if_fail(OSINFO_IS_OS(self), NULL);
    g_return_val_if_fail(OSINFO_IS_HYPERVISOR(hv), NULL);
    g_return_val_if_fail(OSINFO_IS_FILTER(filter), NULL);

    OsinfoDeviceList *newList = osinfo_devicelist_new();
    GList *tmp = NULL;

    if (hv)
        tmp = g_hash_table_lookup(self->priv->hypervisors,
				  osinfo_entity_get_id(OSINFO_ENTITY(hv)));
    else
        tmp = self->priv->deviceLinks;

    while (tmp) {
        struct _OsinfoOsDeviceLink *link = tmp->data;

        if (osinfo_entity_matches_filter(OSINFO_ENTITY(link->dev), filter))
	    osinfo_list_add(OSINFO_LIST(newList), OSINFO_ENTITY(link->dev));

	tmp = tmp->next;
    }

    return NULL;
}


void osinfo_os_add_device(OsinfoOs *self, OsinfoHypervisor *hv, OsinfoDevice *dev, const gchar *driver)
{
    g_return_if_fail(OSINFO_IS_HYPERVISOR(self));
    g_return_if_fail(OSINFO_IS_DEVICE(dev));
    g_return_if_fail(driver != NULL);

    struct _OsinfoOsDeviceLink *link = g_new0(struct _OsinfoOsDeviceLink, 1);

    g_object_ref(dev);
    link->dev = dev;
    link->driver = g_strdup(driver);

    if (hv) {
        GList *tmp = g_hash_table_lookup(self->priv->hypervisors,
					 osinfo_entity_get_id(OSINFO_ENTITY(hv)));
	g_hash_table_steal(self->priv->hypervisors,
			   osinfo_entity_get_id(OSINFO_ENTITY(hv)));
	tmp = g_list_prepend(tmp, link);
	g_hash_table_insert(self->priv->hypervisors,
			    osinfo_entity_get_id(OSINFO_ENTITY(hv)), tmp);
    } else {
        self->priv->deviceLinks = g_list_prepend(self->priv->deviceLinks, link);
    }
}


void osinfo_os_add_related_os(OsinfoOs *self, osinfoRelationship relshp, OsinfoOs *otheros)
{
    g_return_if_fail(OSINFO_IS_OS(self));
    g_return_if_fail(OSINFO_IS_OS(otheros));

    struct _OsinfoOsOsLink *osLink = g_new0(struct _OsinfoOsOsLink, 1);

    g_object_ref(otheros);
    osLink->otherOs = otheros;
    osLink->relshp = relshp;

    self->priv->osLinks = g_list_prepend(self->priv->osLinks, osLink);
}
