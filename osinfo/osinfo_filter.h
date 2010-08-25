/*
 * libosinfo
 *
 * osinfo_filter.h
 * Represents a filter in libosinfo.
 */

#include <glib-object.h>
#include <osinfo/osinfo_entity.h>

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

typedef struct _OsinfoFilter        OsinfoFilter;

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

    gboolean (*matches)(OsinfoFilter *self, OsinfoEntity *entity);
};

GType osinfo_filter_get_type(void);

OsinfoFilter *osinfo_filter_new(void);

gint osinfo_filter_add_constraint(OsinfoFilter *self, gchar *propName, gchar *propVal);

void osinfo_filter_clear_constraint(OsinfoFilter *self, gchar *propName);
void osinfo_filter_clear_constraints(OsinfoFilter *self);

GList *osinfo_filter_get_constraint_keys(OsinfoFilter *self);
GList *osinfo_filter_get_constraint_values(OsinfoFilter *self, gchar *propName);

gboolean osinfo_filter_matches(OsinfoFilter *self,
			       OsinfoEntity *entity);

#endif /* __OSINFO_FILTER_H__ */
