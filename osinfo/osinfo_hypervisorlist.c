#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoHypervisorList, osinfo_hypervisorlist, OSINFO_TYPE_LIST);

#define OSINFO_HYPERVISORLIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_HYPERVISORLIST, OsinfoHypervisorListPrivate))

static void osinfo_hypervisorlist_finalize (GObject *object);

struct _OsinfoHypervisorListPrivate
{
    int tmp;
};

static void
osinfo_hypervisorlist_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_hypervisorlist_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_hypervisorlist_class_init (OsinfoHypervisorListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_hypervisorlist_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoHypervisorListPrivate));
}

static void
osinfo_hypervisorlist_init (OsinfoHypervisorList *self)
{
    OsinfoHypervisorListPrivate *priv;
    self->priv = priv = OSINFO_HYPERVISORLIST_GET_PRIVATE(self);

}
