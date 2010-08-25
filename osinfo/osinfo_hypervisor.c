#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoHypervisor, osinfo_hypervisor, OSINFO_TYPE_ENTITY);

#define OSINFO_HYPERVISOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_HYPERVISOR, OsinfoHypervisorPrivate))

struct _OsinfoHypervisorPrivate
{
    // Value: List of device_link structs
    GList *deviceLinks;
};

struct _OsinfoHypervisorDeviceLink {
    OsinfoDevice *dev;
    gchar *driver;
};

static void osinfo_hypervisor_finalize (GObject *object);

static void osinfo_device_link_free(gpointer data, gpointer opaque G_GNUC_UNUSED)
{
    struct _OsinfoHypervisorDeviceLink *link = data;
    g_object_unref(link->dev);
    g_free(link->driver);
    g_free(link);
}

static void
osinfo_hypervisor_finalize (GObject *object)
{
    OsinfoHypervisor *self = OSINFO_HYPERVISOR (object);

    g_list_foreach(self->priv->deviceLinks, osinfo_device_link_free, NULL);
    g_list_free(self->priv->deviceLinks);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_hypervisor_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_hypervisor_class_init (OsinfoHypervisorClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_hypervisor_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoHypervisorPrivate));
}

static void
osinfo_hypervisor_init (OsinfoHypervisor *self)
{
    OsinfoHypervisorPrivate *priv;
    self->priv = priv = OSINFO_HYPERVISOR_GET_PRIVATE(self);

    self->priv->deviceLinks = NULL;
}

OsinfoHypervisor *osinfo_hypervisor_new(const gchar *id)
{
    return g_object_new(OSINFO_TYPE_HYPERVISOR,
			"id", id,
			NULL);
}


OsinfoDeviceList *osinfo_hypervisor_get_devices(OsinfoHypervisor *self, OsinfoFilter *filter)
{
    g_return_val_if_fail(OSINFO_IS_HYPERVISOR(self), NULL);
    g_return_val_if_fail(OSINFO_IS_FILTER(filter), NULL);

    OsinfoDeviceList *newList = osinfo_devicelist_new();
    GList *tmp = self->priv->deviceLinks;

    while (tmp) {
        struct _OsinfoHypervisorDeviceLink *link = tmp->data;

        if (osinfo_filter_matches(filter, OSINFO_ENTITY(link->dev)))
	    osinfo_list_add(OSINFO_LIST(newList), OSINFO_ENTITY(link->dev));

	tmp = tmp->next;
    }

    return newList;
}

void osinfo_hypervisor_add_device(OsinfoHypervisor *self, OsinfoDevice *dev, const gchar *driver)
{
    g_return_if_fail(OSINFO_IS_HYPERVISOR(self));
    g_return_if_fail(OSINFO_IS_DEVICE(dev));
    g_return_if_fail(driver != NULL);

    struct _OsinfoHypervisorDeviceLink *link = g_new0(struct _OsinfoHypervisorDeviceLink, 1);

    g_object_ref(dev);
    link->dev = dev;
    link->driver = g_strdup(driver);

    self->priv->deviceLinks = g_list_prepend(self->priv->deviceLinks, link);
}
