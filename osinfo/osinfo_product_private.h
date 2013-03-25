/*
 * libosinfo: a software product
 *
 * Copyright (C) 2013 Red Hat, Inc.
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
 *   Christophe Fergeau <cfergeau@redhat.com>
 */

#include "osinfo_product.h"

#ifndef __OSINFO_PRODUCT_PRIVATE_H__
#define __OSINFO_PRODUCT_PRIVATE_H__


typedef void (*OsinfoProductForeach)(OsinfoProduct *product, gpointer user_data);

typedef enum { /*< skip >*/
    OSINFO_PRODUCT_FOREACH_FLAG_DERIVES_FROM = 1 << 0,
    OSINFO_PRODUCT_FOREACH_FLAG_UPGRADES = 1 << 1,
    OSINFO_PRODUCT_FOREACH_FLAG_CLONES = 1 << 2,
} OsinfoProductForeachFlag;

void osinfo_product_foreach_related(OsinfoProduct *product,
                                    OsinfoProductForeachFlag flags,
                                    OsinfoProductForeach foreach_func,
                                    gpointer user_data);

#endif /* __OSINFO_PRODUCT_PRIVATE_H__ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
