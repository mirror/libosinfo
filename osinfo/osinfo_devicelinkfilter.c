/*
 * libosinfo:
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

#include <config.h>

#include <osinfo/osinfo.h>
#include <glib/gi18n-lib.h>

G_DEFINE_TYPE (OsinfoDeviceLinkFilter, osinfo_devicelinkfilter, OSINFO_TYPE_FILTER);

#define OSINFO_DEVICELINKFILTER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_DEVICELINKFILTER, OsinfoDeviceLinkFilterPrivate))

/**
 * SECTION:osinfo_devicelinkfilter
 * @short_description: an operating system filter
 * @see_also: #OsinfoFilter, #OsinfoDeviceLink
 *
 * #OsinfoDeviceLinkFilter is a specialization of #OsinfoFilter that
 * can also set a constraint against the device link target.
 * It can only be used to filter entities
 * that are #OsinfoDeviceLink objects.
 */

struct _OsinfoDeviceLinkFilterPrivate
{
    OsinfoFilter *targetFilter;
};

static gboolean osinfo_devicelinkfilter_matches_default(OsinfoFilter *devicelinkfilter, OsinfoEntity *entity);

enum {
    PROP_0,

    PROP_TARGET_FILTER,
};


static void
osinfo_devicelinkfilter_set_property(GObject *object,
                                     guint property_id,
                                     const GValue *value,
                                     GParamSpec *pspec)
{
    OsinfoDeviceLinkFilter *filter = OSINFO_DEVICELINKFILTER (object);

    switch (property_id)
        {
        case PROP_TARGET_FILTER:
            if (filter->priv->targetFilter)
                g_object_unref(filter->priv->targetFilter);
            filter->priv->targetFilter = g_value_get_object(value);
            if (filter->priv->targetFilter)
                g_object_ref(filter->priv->targetFilter);
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
        }
}

static void
osinfo_devicelinkfilter_get_property(GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec)
{
    OsinfoDeviceLinkFilter *filter = OSINFO_DEVICELINKFILTER(object);

    switch (property_id)
        {
        case PROP_TARGET_FILTER:
            g_value_set_object(value, filter->priv->targetFilter);
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
        }
}


static void
osinfo_devicelinkfilter_finalize (GObject *object)
{
    OsinfoDeviceLinkFilter *filter = OSINFO_DEVICELINKFILTER (object);

    g_object_unref(filter->priv->targetFilter);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_devicelinkfilter_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_devicelinkfilter_class_init (OsinfoDeviceLinkFilterClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS(klass);
    OsinfoFilterClass *filter_klass = OSINFO_FILTER_CLASS(klass);
    GParamSpec *pspec;

    g_klass->finalize = osinfo_devicelinkfilter_finalize;
    g_klass->set_property = osinfo_devicelinkfilter_set_property;
    g_klass->get_property = osinfo_devicelinkfilter_get_property;

    /**
     * OsinfoDeviceLinkFilter:target-filter:
     *
     * The operating system to be deployed
     */
    pspec = g_param_spec_object("target-filter",
                                "Target Filter",
                                _("Device link target filter"),
                                OSINFO_TYPE_FILTER,
                                G_PARAM_CONSTRUCT_ONLY |
                                G_PARAM_READWRITE |
                                G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(g_klass,
                                    PROP_TARGET_FILTER,
                                    pspec);

    g_type_class_add_private (klass, sizeof (OsinfoDeviceLinkFilterPrivate));

    filter_klass->matches = osinfo_devicelinkfilter_matches_default;
}


/**
 * osinfo_devicelinkfilter_new:
 * @filter: the target device filter
 *
 * Construct a new filter that matches all operating
 * systems
 *
 * Returns: (transfer full): a new filter
 */
OsinfoDeviceLinkFilter *osinfo_devicelinkfilter_new(OsinfoFilter *filter)
{
    return g_object_new(OSINFO_TYPE_DEVICELINKFILTER,
                        "target-filter", filter,
                        NULL);
}


static void
osinfo_devicelinkfilter_init (OsinfoDeviceLinkFilter *devicelinkfilter)
{
    devicelinkfilter->priv = OSINFO_DEVICELINKFILTER_GET_PRIVATE(devicelinkfilter);
}


/**
 * osinfo_devicelinkfilter_get_target_filter:
 * @filter: the filter object
 *
 * Retrieve the filter used to match against the target of
 * the device link
 *
 * Returns: (transfer none): the target filter object
 */
OsinfoFilter *osinfo_devicelinkfilter_get_target_filter(OsinfoDeviceLinkFilter *filter)
{
    return filter->priv->targetFilter;
}


static gboolean osinfo_devicelinkfilter_matches_default(OsinfoFilter *filter, OsinfoEntity *entity)
{
    g_return_val_if_fail(OSINFO_IS_DEVICELINKFILTER(filter), FALSE);
    g_return_val_if_fail(OSINFO_IS_DEVICELINK(entity), FALSE);
    OsinfoDeviceLinkFilter *linkfilter = OSINFO_DEVICELINKFILTER(filter);

    if (!OSINFO_FILTER_CLASS (osinfo_devicelinkfilter_parent_class)->matches(filter, entity))
        return FALSE;

    if (!osinfo_filter_matches(linkfilter->priv->targetFilter,
                               OSINFO_ENTITY(osinfo_devicelink_get_target(OSINFO_DEVICELINK(entity)))))
        return FALSE;

    return TRUE;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
