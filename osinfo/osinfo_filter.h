/*
 * libosinfo: a mechanism to filter enities
 *
 * Copyright (C) 2009-2012 Red Hat, Inc.
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
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
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
    /*< private >*/
    GObjectClass parent_class;

    /* class members */

    gboolean (*matches)(OsinfoFilter *filter, OsinfoEntity *entity);
};

GType osinfo_filter_get_type(void);

OsinfoFilter *osinfo_filter_new(void);

void osinfo_filter_add_constraint(OsinfoFilter *filter,
                                  const gchar *propName,
                                  const gchar *propVal);

void osinfo_filter_clear_constraint(OsinfoFilter *filter,
                                    const gchar *propName);
void osinfo_filter_clear_constraints(OsinfoFilter *filter);

GList *osinfo_filter_get_constraint_keys(OsinfoFilter *filter);
GList *osinfo_filter_get_constraint_values(OsinfoFilter *filter,
                                           const gchar *propName);

gboolean osinfo_filter_matches(OsinfoFilter *filter,
                               OsinfoEntity *entity);

#endif /* __OSINFO_FILTER_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
