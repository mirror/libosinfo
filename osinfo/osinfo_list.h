/*
 * libosinfo: a list of entities
 *
 * Copyright (C) 2009-2010 Red Hat, Inc
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 * Authors:
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
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

typedef struct _OsinfoList       OsinfoList;

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

GType osinfo_list_get_element_type(OsinfoList *list);
gint osinfo_list_get_length(OsinfoList *list);
OsinfoEntity *osinfo_list_get_nth(OsinfoList *list, gint idx);
OsinfoEntity *osinfo_list_find_by_id(OsinfoList *list, const gchar *id);

void osinfo_list_add(OsinfoList *list, OsinfoEntity *entity);
void osinfo_list_add_filtered(OsinfoList *list, OsinfoList *source, OsinfoFilter *filter);
void osinfo_list_add_intersection(OsinfoList *list, OsinfoList *sourceOne, OsinfoList *sourceTwo);
void osinfo_list_add_union(OsinfoList *list, OsinfoList *sourceOne, OsinfoList *sourceTwo);
void osinfo_list_add_all(OsinfoList *list, OsinfoList *source);

typedef gboolean (*osinfo_list_iterator)(OsinfoList *list, OsinfoEntity *entity, gpointer data);
void osinfo_list_foreach(OsinfoList *list, osinfo_list_iterator iter, gpointer data);

#endif /* __OSINFO_LIST_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
