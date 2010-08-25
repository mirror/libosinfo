/*
 * libosinfo:
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

#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoDevice, osinfo_device, OSINFO_TYPE_ENTITY);

#define OSINFO_DEVICE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_DEVICE, OsinfoDevicePrivate))

struct _OsinfoDevicePrivate
{
    gboolean unused;
};


static void osinfo_device_finalize (GObject *object);

static void
osinfo_device_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_device_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_device_class_init (OsinfoDeviceClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_device_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoDevicePrivate));
}

static void
osinfo_device_init (OsinfoDevice *self)
{
    OsinfoDevicePrivate *priv;
    self->priv = priv = OSINFO_DEVICE_GET_PRIVATE(self);
}

OsinfoDevice *osinfo_device_new(const gchar *id)
{
    return g_object_new(OSINFO_TYPE_DEVICE,
			"id", id,
			NULL);
}
