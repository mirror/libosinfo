/*
 * libosinfo
 *
 * osinfo_db.h
 * Represents the main entry point to data contained by libosinfo.
 */

#ifndef __OSINFO_DB_H__
#define __OSINFO_DB_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_DB                  (osinfo_db_get_type ())
#define OSINFO_DB(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_DB, OsinfoDb))
#define OSINFO_IS_DB(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_DB))
#define OSINFO_DB_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_DB, OsinfoDbClass))
#define OSINFO_IS_DB_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_DB))
#define OSINFO_DB_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_DB, OsinfoDbClass))

//typedef struct _OsinfoDb        OsinfoDb;
// (defined in osinfo_objects.h)

typedef struct _OsinfoDbClass   OsinfoDbClass;

typedef struct _OsinfoDbPrivate OsinfoDbPrivate;

/*
 * To get a db handle, we construct one with a construct-time only
 * backing data directory. It is already considered to be initialized
 * on return from the constructor, and ready to do work.
 *
 * To close it, we call the destructor on it.
 * Setting parameters on it will work if it's not a construct-time only
 * parameter. Reading will always work. Currently the backing directory and
 * libvirt version are the only parameters.
 *
 * The db object contains information related to three main classes of
 * objects: hypervisors, operating systems and devices.
 */

/* object */
struct _OsinfoDb
{
    GObject parent_instance;

    /* public */

    /* private */
    OsinfoDbPrivate *priv;
};

/* class */
struct _OsinfoDbClass
{
    GObjectClass parent_class;

    /* class members */
};

GType osinfo_db_get_type(void);

OsinfoDb *osinfo_db_new(const gchar *backingDir);

int osinfo_db_initialize(OsinfoDb *self, GError **err);

OsinfoHypervisor *osinfo_db_get_hypervisor(OsinfoDb *self, gchar *hvId);
OsinfoDevice *osinfo_db_get_device(OsinfoDb *self, gchar *devId);
OsinfoOs *osinfo_db_get_os(OsinfoDb *self, gchar *osId);

OsinfoOsList *osinfo_db_get_os_list(OsinfoDb *self);
OsinfoHypervisorList *osinfo_db_get_hypervisor_list(OsinfoDb *self);
OsinfoDeviceList *osinfo_db_get_device_list(OsinfoDb *self);

// Get me all unique values for property "vendor" among operating systems
GPtrArray *osinfo_db_unique_values_for_property_in_os(OsinfoDb *self, gchar *propName);

// Get me all unique values for property "vendor" among hypervisors
GPtrArray *osinfo_db_unique_values_for_property_in_hv(OsinfoDb *self, gchar *propName);

// Get me all unique values for property "vendor" among devices
GPtrArray *osinfo_db_unique_values_for_property_in_dev(OsinfoDb *self, gchar *propName);

// Get me all OSes that 'upgrade' another OS (or whatever relationship is specified)
OsinfoOsList *osinfo_db_unique_values_for_os_relationship(OsinfoDb *self, osinfoRelationship relshp);


#endif /* __OSINFO_DB_H__ */
