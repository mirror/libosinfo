/*
 * libosinfo: OS installation script
 *
 * Copyright (C) 2012 Red Hat, Inc.
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
 *   Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
 */

#include <osinfo/osinfo_install_script.h>
#include <osinfo/osinfo_avatar_format.h>

#ifndef __OSINFO_INSTALL_SCRIPT_PRIVATE_H__
#define __OSINFO_INSTALL_SCRIPT_PRIVATE_H__

void osinfo_install_script_add_config_param(OsinfoInstallScript *script, OsinfoInstallConfigParam *param);

void osinfo_install_script_set_avatar_format(OsinfoInstallScript *script,
                                             OsinfoAvatarFormat *avatar);

#endif /* __OSINFO_INSTALL_SCRIPT_PRIVATE_H__ */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
