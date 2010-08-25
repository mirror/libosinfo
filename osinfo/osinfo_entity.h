/*
 * libosinfo
 *
 * osinfo_entity.h
 * All entities represented in libosinfo are derived from this class.
 */

#include <glib-object.h>

#ifndef __OSINFO_ENTITY_H__
#define __OSINFO_ENTITY_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_ENTITY                  (osinfo_entity_get_type ())
#define OSINFO_ENTITY(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_ENTITY, OsinfoEntity))
#define OSINFO_IS_ENTITY(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_ENTITY))
#define OSINFO_ENTITY_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_ENTITY, OsinfoEntityClass))
#define OSINFO_IS_ENTITY_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_ENTITY))
#define OSINFO_ENTITY_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_ENTITY, OsinfoEntityClass))

typedef struct _OsinfoEntity        OsinfoEntity;

typedef struct _OsinfoEntityClass   OsinfoEntityClass;

typedef struct _OsinfoEntityPrivate OsinfoEntityPrivate;

/* object */
struct _OsinfoEntity
{
    GObject parent_instance;

    /* public */

    /* private */
    OsinfoEntityPrivate *priv;
};

/* class */
struct _OsinfoEntityClass
{
    GObjectClass parent_class;

    /* class members */
};

GType osinfo_entity_get_type(void);

gchar *osinfo_entity_get_id(OsinfoEntity *self);
GList *osinfo_entity_get_param_keys(OsinfoEntity *self);
gchar *osinfo_entity_get_param_value(OsinfoEntity *self, const gchar *key);
GList *osinfo_entity_get_param_value_list(OsinfoEntity *self, const gchar *key);
void osinfo_entity_add_param(OsinfoEntity *self, const gchar *key, const gchar *value);
void osinfo_entity_clear_param(OsinfoEntity *self, const gchar *key);

#endif /* __OSINFO_ENTITY_H__ */
