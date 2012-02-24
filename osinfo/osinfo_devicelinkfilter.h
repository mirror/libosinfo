/*
 * libosinfo: a mechanism to filter device links
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

#ifndef __OSINFO_DEVICELINKFILTER_H__
#define __OSINFO_DEVICELINKFILTER_H__

/*
 * Type macros.
 */
#define OSINFO_TYPE_DEVICELINKFILTER                  (osinfo_devicelinkfilter_get_type ())
#define OSINFO_DEVICELINKFILTER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), OSINFO_TYPE_DEVICELINKFILTER, OsinfoDeviceLinkFilter))
#define OSINFO_IS_DEVICELINKFILTER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSINFO_TYPE_DEVICELINKFILTER))
#define OSINFO_DEVICELINKFILTER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), OSINFO_TYPE_DEVICELINKFILTER, OsinfoDeviceLinkFilterClass))
#define OSINFO_IS_DEVICELINKFILTER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), OSINFO_TYPE_DEVICELINKFILTER))
#define OSINFO_DEVICELINKFILTER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), OSINFO_TYPE_DEVICELINKFILTER, OsinfoDeviceLinkFilterClass))

typedef struct _OsinfoDeviceLinkFilter        OsinfoDeviceLinkFilter;

typedef struct _OsinfoDeviceLinkFilterClass  OsinfoDeviceLinkFilterClass;

typedef struct _OsinfoDeviceLinkFilterPrivate OsinfoDeviceLinkFilterPrivate;

/* object */
struct _OsinfoDeviceLinkFilter
{
    OsinfoFilter parent_instance;

    /* public */

    /* private */
    OsinfoDeviceLinkFilterPrivate *priv;
};

/* class */
struct _OsinfoDeviceLinkFilterClass
{
    OsinfoFilterClass parent_class;

    /* class members */
};

GType osinfo_devicelinkfilter_get_type(void);

OsinfoDeviceLinkFilter *osinfo_devicelinkfilter_new(OsinfoFilter *filter);

OsinfoFilter *osinfo_devicelinkfilter_get_target_filter(OsinfoDeviceLinkFilter *filter);

#endif /* __OSINFO_DEVICELINKFILTER_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
