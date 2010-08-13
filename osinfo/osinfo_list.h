/*
 * libosinfo
 *
 * osinfo_list.h
 */

#ifndef __OSINFO_LIST_H__
#define __OSINFO_LIST_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_LIST                  (osinfo_list_get_type ())
#define OSINFO_LIST(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_LIST, OsinfoList))
#define OSINFO_IS_LIST(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_LIST))
#define OSINFO_LIST_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_LIST, OsinfoListClass))
#define OSINFO_IS_LIST_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_LIST))
#define OSINFO_LIST_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_LIST, OsinfoListClass))

typedef struct _OsinfoListClass   OsinfoListClass;

typedef struct _OsinfoListPrivate OsinfoListPrivate;

/* object */
struct _OsinfoList
{
    GObject parent_instance;

    /* public */

    /* private */
    OsinfoListPrivate *priv;
};

/* class */
struct _OsinfoListClass
{
    GObjectClass parent_class;

    /* class members */
};

GType osinfo_list_get_type(void);

gint osinfo_list_get_length(OsinfoList *self);
OsinfoEntity *osinfo_list_get_nth(OsinfoList *self, gint idx);
OsinfoEntity *osinfo_list_find_by_id(OsinfoList *self, const gchar *id);

void osinfo_list_add(OsinfoList *self, OsinfoEntity *entity);
void osinfo_list_add_filtered(OsinfoList *self, OsinfoList *source, OsinfoFilter *filter);
void osinfo_list_add_intersection(OsinfoList *self, OsinfoList *sourceOne, OsinfoList *sourceTwo);
void osinfo_list_add_union(OsinfoList *self, OsinfoList *sourceOne, OsinfoList *sourceTwo);

typedef gboolean (*osinfo_list_iterator)(OsinfoList *self, OsinfoEntity *entity, gpointer data);
void osinfo_list_foreach(OsinfoList *self, osinfo_list_iterator iter, gpointer data);

#endif /* __OSINFO_LIST_H__ */
