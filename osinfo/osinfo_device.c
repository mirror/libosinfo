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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Authors:
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoDevice, osinfo_device, OSINFO_TYPE_ENTITY);

#define OSINFO_DEVICE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_DEVICE, OsinfoDevicePrivate))

/**
 * SECTION:osinfo_device
 * @short_description: A hardware device
 * @see_also: #OsinfoOs, #OsinfoPlatform
 *
 * #OsinfoDevice is an entity representing some kind of hardware
 * device. Devices can be associated with operating systems
 * and platforms.
 */

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
osinfo_device_init (OsinfoDevice *device)
{
    OsinfoDevicePrivate *priv;
    device->priv = priv = OSINFO_DEVICE_GET_PRIVATE(device);
}

OsinfoDevice *osinfo_device_new(const gchar *id)
{
    return g_object_new(OSINFO_TYPE_DEVICE,
                        "id", id,
                        NULL);
}


const gchar *osinfo_device_get_vendor(OsinfoDevice *dev)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(dev), OSINFO_DEVICE_PROP_VENDOR);
}

const gchar *osinfo_device_get_product(OsinfoDevice *dev)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(dev), OSINFO_DEVICE_PROP_PRODUCT);
}

const gchar *osinfo_device_get_bus_type(OsinfoDevice *dev)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(dev), OSINFO_DEVICE_PROP_BUS_TYPE);
}

const gchar *osinfo_device_get_class(OsinfoDevice *dev)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(dev), OSINFO_DEVICE_PROP_CLASS);
}

const gchar *osinfo_device_get_name(OsinfoDevice *dev)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(dev), OSINFO_DEVICE_PROP_NAME);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
