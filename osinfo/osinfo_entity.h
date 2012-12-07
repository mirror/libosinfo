/*
 * libosinfo: an object with a set of parameters
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
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Authors:
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
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

#define OSINFO_ENTITY_PROP_ID "id"

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
    /*< private >*/
    GObjectClass parent_class;

    /* class members */
};

GType osinfo_entity_get_type(void);

const gchar *osinfo_entity_get_id(OsinfoEntity *entity);

GList *osinfo_entity_get_param_keys(OsinfoEntity *entity);
const gchar *osinfo_entity_get_param_value(OsinfoEntity *entity, const gchar *key);
gboolean osinfo_entity_get_param_value_boolean(OsinfoEntity *entity, const gchar *key);
gboolean osinfo_entity_get_param_value_boolean_with_default(OsinfoEntity *entity,
                                                            const char *key,
                                                            gboolean default_value);
gint osinfo_entity_get_param_value_enum(OsinfoEntity *entity,
                                        const char *key,
                                        GType enum_type,
                                        gint default_value);
gint64 osinfo_entity_get_param_value_int64(OsinfoEntity *entity, const gchar *key);
gint64 osinfo_entity_get_param_value_int64_with_default(OsinfoEntity *entity,
                                                        const gchar *key,
                                                        gint64 default_value);
GList *osinfo_entity_get_param_value_list(OsinfoEntity *entity, const gchar *key);
void osinfo_entity_set_param(OsinfoEntity *entity, const gchar *key, const gchar *value);
void osinfo_entity_set_param_boolean(OsinfoEntity *entity, const gchar *key, gboolean value);
void osinfo_entity_set_param_int64(OsinfoEntity *entity, const gchar *key, gint64 value);
void osinfo_entity_set_param_enum(OsinfoEntity *entity, const gchar *key, gint value, GType enum_type);
void osinfo_entity_add_param(OsinfoEntity *entity, const gchar *key, const gchar *value);
void osinfo_entity_clear_param(OsinfoEntity *entity, const gchar *key);

#endif /* __OSINFO_ENTITY_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
