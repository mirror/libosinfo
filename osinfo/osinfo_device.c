#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoDevice, osinfo_device, OSINFO_TYPE_ENTITY);

#define OSINFO_DEVICE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_DEVICE, OsinfoDevicePrivate))

struct _OsinfoDevicePrivate
{
    gboolean unused;
};


static void osinfo_device_finalize (GObject *object);

static void
osinfo_device_finalize (GObject *object)
{
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
