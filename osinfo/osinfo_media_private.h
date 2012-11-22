/*
 * libosinfo: An installation media for a (guest) OS
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
 *   Christophe Fergeau <cfergeau@redhat.com>
 */

#include <osinfo/osinfo_media.h>

#ifndef __OSINFO_MEDIA_PRIVATE_H__
#define __OSINFO_MEDIA_PRIVATE_H__

void osinfo_media_set_os(OsinfoMedia *media, OsinfoOs *os);

#endif /* __OSINFO_MEDIA_PRIVATE_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
