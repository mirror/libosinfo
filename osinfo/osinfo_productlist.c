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

G_DEFINE_TYPE (OsinfoProductList, osinfo_productlist, OSINFO_TYPE_LIST);

#define OSINFO_PRODUCTLIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_PRODUCTLIST, OsinfoProductListPrivate))

/**
 * SECTION:osinfo_productlist
 * @short_description: A list of software products
 * @see_also: #OsinfoList, #OsinfoProduct
 *
 * #OsinfoProductList is a list specialization that stores
 * only #OsinfoProduct objects.
 */

struct _OsinfoProductListPrivate
{
    gboolean unused;
};

static void
osinfo_productlist_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_productlist_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_productlist_class_init (OsinfoProductListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_productlist_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoProductListPrivate));
}

static void
osinfo_productlist_init (OsinfoProductList *list)
{
    OsinfoProductListPrivate *priv;
    list->priv = priv = OSINFO_PRODUCTLIST_GET_PRIVATE(list);

}


/**
 * osinfo_productlist_new:
 *
 * Construct a new os list that is initially empty.
 *
 * Returns: (transfer full): an empty os list
 */
OsinfoProductList *osinfo_productlist_new(void)
{
    return g_object_new(OSINFO_TYPE_PRODUCTLIST,
			"element-type", OSINFO_TYPE_PRODUCT,
			NULL);
}


/**
 * osinfo_productlist_new_copy:
 * @source: the os list to copy
 *
 * Construct a new os list that is filled with oss
 * from @source
 *
 * Returns: (transfer full): a copy of the os list
 */
OsinfoProductList *osinfo_productlist_new_copy(OsinfoProductList *source)
{
    OsinfoProductList *newList = osinfo_productlist_new();
    osinfo_list_add_all(OSINFO_LIST(newList),
			OSINFO_LIST(source));
    return newList;
}

/**
 * osinfo_productlist_new_filtered:
 * @source: the os list to copy
 * @filter: the filter to apply
 *
 * Construct a new os list that is filled with oss
 * from @source that match @filter
 *
 * Returns: (transfer full): a filtered copy of the os list
 */
OsinfoProductList *osinfo_productlist_new_filtered(OsinfoProductList *source, OsinfoFilter *filter)
{
    OsinfoProductList *newList = osinfo_productlist_new();
    osinfo_list_add_filtered(OSINFO_LIST(newList),
			     OSINFO_LIST(source),
			     filter);
    return newList;
}

/**
 * osinfo_productlist_new_intersection:
 * @sourceOne: the first os list to copy
 * @sourceTwo: the second os list to copy
 *
 * Construct a new os list that is filled with only the
 * oss that are present in both @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): an intersection of the two os lists
 */
OsinfoProductList *osinfo_productlist_new_intersection(OsinfoProductList *sourceOne, OsinfoProductList *sourceTwo)
{
    OsinfoProductList *newList = osinfo_productlist_new();
    osinfo_list_add_intersection(OSINFO_LIST(newList),
				 OSINFO_LIST(sourceOne),
				 OSINFO_LIST(sourceTwo));
    return newList;
}

/**
 * osinfo_productlist_new_union:
 * @sourceOne: the first os list to copy
 * @sourceTwo: the second os list to copy
 *
 * Construct a new os list that is filled with all the
 * oss that are present in either @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): a union of the two os lists
 */
OsinfoProductList *osinfo_productlist_new_union(OsinfoProductList *sourceOne, OsinfoProductList *sourceTwo)
{
    OsinfoProductList *newList = osinfo_productlist_new();
    osinfo_list_add_union(OSINFO_LIST(newList),
			  OSINFO_LIST(sourceOne),
			  OSINFO_LIST(sourceTwo));
    return newList;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
