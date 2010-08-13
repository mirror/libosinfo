#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoOsList, osinfo_oslist, OSINFO_TYPE_LIST);

#define OSINFO_OSLIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_OSLIST, OsinfoOsListPrivate))

static void osinfo_oslist_finalize (GObject *object);

struct _OsinfoOsListPrivate
{
    int tmp;
};

static void
osinfo_oslist_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_oslist_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_oslist_class_init (OsinfoOsListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_oslist_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoOsListPrivate));
}

static void
osinfo_oslist_init (OsinfoOsList *self)
{
    OsinfoOsListPrivate *priv;
    self->priv = priv = OSINFO_OSLIST_GET_PRIVATE(self);

}

