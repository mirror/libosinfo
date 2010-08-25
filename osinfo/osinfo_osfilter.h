/*
 * libosinfo
 *
 * osinfo_osfilter.h
 * Represents a osfilter in libosinfo.
 */

#ifndef __OSINFO_OSFILTER_H__
#define __OSINFO_OSFILTER_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_OSFILTER                  (osinfo_osfilter_get_type ())
#define OSINFO_OSFILTER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_OSFILTER, OsinfoOsfilter))
#define OSINFO_IS_OSFILTER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_OSFILTER))
#define OSINFO_OSFILTER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_OSFILTER, OsinfoOsfilterClass))
#define OSINFO_IS_OSFILTER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_OSFILTER))
#define OSINFO_OSFILTER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_OSFILTER, OsinfoOsfilterClass))

typedef struct _OsinfoOsfilter        OsinfoOsfilter;

typedef struct _OsinfoOsfilterClass  OsinfoOsfilterClass;

typedef struct _OsinfoOsfilterPrivate OsinfoOsfilterPrivate;

/* object */
struct _OsinfoOsfilter
{
    OsinfoFilter parent_instance;

    /* public */

    /* private */
    OsinfoOsfilterPrivate *priv;
};

/* class */
struct _OsinfoOsfilterClass
{
    OsinfoFilterClass parent_class;

    /* class members */
};

GType osinfo_osfilter_get_type(void);

OsinfoOsfilter *osinfo_osfilter_new(void);

gint osinfo_osfilter_add_os_constraint(OsinfoOsfilter *self, OsinfoOsRelationship relshp, OsinfoOs *os);
void osinfo_osfilter_clear_os_constraint(OsinfoOsfilter *self, OsinfoOsRelationship relshp);
void osinfo_osfilter_clear_os_constraints(OsinfoOsfilter *self);

GList *osinfo_osfilter_get_os_constraint_values(OsinfoOsfilter *self, OsinfoOsRelationship relshp);

#endif /* __OSINFO_OSFILTER_H__ */
