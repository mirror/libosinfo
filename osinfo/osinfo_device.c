#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoDevice, osinfo_device, OSINFO_TYPE_ENTITY);

#define OSINFO_DEVICE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_DEVICE, OsinfoDevicePrivate))

static void osinfo_device_finalize (GObject *object);

static void
osinfo_device_finalize (GObject *object)
{
    OsinfoDevice *self = OSINFO_DEVICE (object);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_device_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_device_class_init (OsinfoDeviceClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_device_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoDevicePrivate));
}

static void
osinfo_device_init (OsinfoDevice *self)
{
    OsinfoDevicePrivate *priv;
    self->priv = priv = OSINFO_DEVICE_GET_PRIVATE(self);
}

OsinfoDevice *osinfo_device_new(const gchar *id)
{
    return g_object_new(OSINFO_TYPE_DEVICE,
			"id", id,
			NULL);
}

gchar *osinfo_device_get_driver(OsinfoDevice *self,
				gchar *devType,
				OsinfoOs *os,
				OsinfoHypervisor *hv)
{
    g_return_val_if_fail(OSINFO_IS_DEVICE(self), NULL);
    g_return_val_if_fail(OSINFO_IS_OS(os), NULL);
    g_return_val_if_fail(OSINFO_IS_HYPERVISOR(hv), NULL);
    g_return_val_if_fail(devType != NULL, NULL);

    gchar *driver = NULL;

    // For os, get hypervisor specific info. If not present, return NULL.
    struct __osinfoHvSection *hvSection = NULL;
    hvSection = g_tree_lookup(os->priv->hypervisors, (OSINFO_ENTITY(hv))->priv->id);
    if (!hvSection)
        return NULL;

    // Check for info for type of devices in <os,hv>. If not found, return NULL.
    GTree *section = NULL;
    section = g_tree_lookup(hvSection->sections, devType);
    if (!section)
        return NULL;

    // Check device section for device. If not found, return NULL.
    struct __osinfoDeviceLink *deviceLink = NULL;
    deviceLink = g_tree_lookup(section, (OSINFO_ENTITY(self))->priv->id);
    if (!deviceLink)
        return NULL;

    if (!deviceLink->driver)
        return NULL;

    driver = g_strdup(deviceLink->driver);

    return driver;
}
