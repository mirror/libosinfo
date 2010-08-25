/*
 * libosinfo
 *
 * osinfo_filter.h
 * Represents a filter in libosinfo.
 */

#ifndef __OSINFO_FILTER_H__
#define __OSINFO_FILTER_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_FILTER                  (osinfo_filter_get_type ())
#define OSINFO_FILTER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_FILTER, OsinfoFilter))
#define OSINFO_IS_FILTER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_FILTER))
#define OSINFO_FILTER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_FILTER, OsinfoFilterClass))
#define OSINFO_IS_FILTER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_FILTER))
#define OSINFO_FILTER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_FILTER, OsinfoFilterClass))

//typedef struct _OsinfoFilter        OsinfoFilter;

typedef struct _OsinfoFilterClass  OsinfoFilterClass;

typedef struct _OsinfoFilterPrivate OsinfoFilterPrivate;

/* object */
struct _OsinfoFilter
{
    GObject parent_instance;

    /* public */

    /* private */
    OsinfoFilterPrivate *priv;
};

/* class */
struct _OsinfoFilterClass
{
    GObjectClass parent_class;

    /* class members */
};

GType osinfo_filter_get_type(void);

OsinfoFilter *osinfo_filter_new(void);

gint osinfo_filter_add_constraint(OsinfoFilter *self, gchar *propName, gchar *propVal);

// Only applicable to OSes, ignored by other types of objects
gint osinfo_filter_add_relation_constraint(OsinfoFilter *self, OsinfoOsRelationship relshp, OsinfoOs *os);

void osinfo_filter_clear_constraint(OsinfoFilter *self, gchar *propName);
void osinfo_filter_clear_relationship_constraint(OsinfoFilter *self, OsinfoOsRelationship relshp);
void osinfo_filter_clear_all_constraints(OsinfoFilter *self);

GList *osinfo_filter_get_constraint_keys(OsinfoFilter *self);
GList *osinfo_filter_get_constraint_values(OsinfoFilter *self, gchar *propName);
OsinfoOsList *osinfo_filter_get_relationship_constraint_value(OsinfoFilter *self, OsinfoOsRelationship relshp);

gboolean osinfo_filter_matches(OsinfoFilter *self,
			       OsinfoEntity *entity);

#endif /* __OSINFO_FILTER_H__ */
