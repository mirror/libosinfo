/*
 * osinfo:
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPPRODUCTE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Bproductton, MA 02111-1307  USA
 *
 * Authors:
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <osinfo/osinfo.h>

G_DEFINE_ABSTRACT_TYPE (OsinfoProduct, osinfo_product, OSINFO_TYPE_ENTITY);

#define OSINFO_PRODUCT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_PRODUCT, OsinfoProductPrivate))

/**
 * SECTION:osinfo_product
 * @short_description: An software product
 * @see_also: #OsinfoProduct, #OsinfoPlatform
 *
 * #OsinfoProduct is an entity representing an software
 * product. There are relationships amongst products
 * to declare which are newest releases, which are clones
 * and which are derived from a common ancestry.
 */

/**
 * OsinfoProductRelationship:
 * @OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM: a descendent (RHEL-5 derives from Fedora-6)
 * @OSINFO_PRODUCT_RELATIONSHIP_CLONES: a perfect clone (CentPRODUCT-5 clones RHEL-5)
 * @OSINFO_PRODUCT_RELATIONSHIP_UPGRADES: a new version release (RHEL-6 upgrades RHEL-4)
 *
 * Enum values used to form relationships between products
 */
struct _OsinfoProductPrivate
{
    // Value: Array of product_link structs
    GList *productLinks;
};

struct _OsinfoProductProductLink {
    /* <product> 'verbs' <other_product>
     * fedora11 upgrades fedora10
     * centproduct clones rhel
     * scientificlinux derives from rhel
     */
    OsinfoProductRelationship relshp;
    OsinfoProduct *otherProduct;
};

static void osinfo_product_link_free(gpointer data, gpointer opaque G_GNUC_UNUSED)
{
    struct _OsinfoProductProductLink *link = data;
    g_object_unref(link->otherProduct);
    g_free(link);
}


static void
osinfo_product_finalize (GObject *object)
{
    OsinfoProduct *product = OSINFO_PRODUCT (object);

    g_list_foreach(product->priv->productLinks, osinfo_product_link_free, NULL);
    g_list_free(product->priv->productLinks);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_product_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_product_class_init (OsinfoProductClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_product_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoProductPrivate));
}

static void
osinfo_product_init (OsinfoProduct *product)
{
    OsinfoProductPrivate *priv;
    product->priv = priv = OSINFO_PRODUCT_GET_PRIVATE(product);

    product->priv->productLinks = NULL;
}


/**
 * osinfo_product_get_related:
 * @product: an product
 * @relshp: the relationship to query
 *
 * Get a list of products satisfying the the requested
 * relationship
 *
 * Returns: (transfer full): a list of related products
 */
OsinfoProductList *osinfo_product_get_related(OsinfoProduct *product, OsinfoProductRelationship relshp)
{
    g_return_val_if_fail(OSINFO_IS_PRODUCT(product), NULL);

    // Create our list
    OsinfoProductList *newList = osinfo_productlist_new();
    GList *tmp = product->priv->productLinks;

    while (tmp) {
        struct _OsinfoProductProductLink *link = tmp->data;

        if (link->relshp == relshp)
            osinfo_list_add(OSINFO_LIST(newList), OSINFO_ENTITY(link->otherProduct));

        tmp = tmp->next;
    }

    return newList;
}


/**
 * osinfo_product_add_related:
 * @product: a product
 * @relshp: the relationship
 * @otherproduct: (transfer none): the product to relate to
 *
 * Add an association between two products
 */
void osinfo_product_add_related(OsinfoProduct *product, OsinfoProductRelationship relshp, OsinfoProduct *otherproduct)
{
    g_return_if_fail(OSINFO_IS_PRODUCT(product));
    g_return_if_fail(OSINFO_IS_PRODUCT(otherproduct));

    struct _OsinfoProductProductLink *productLink = g_new0(struct _OsinfoProductProductLink, 1);

    g_object_ref(otherproduct);
    productLink->otherProduct = otherproduct;
    productLink->relshp = relshp;

    product->priv->productLinks = g_list_prepend(product->priv->productLinks, productLink);
}

const gchar *osinfo_product_get_vendor(OsinfoProduct *prod)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(prod), OSINFO_PRODUCT_PROP_VENDOR);
}
const gchar *osinfo_product_get_version(OsinfoProduct *prod)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(prod), OSINFO_PRODUCT_PROP_VERSION);
}
const gchar *osinfo_product_get_short_id(OsinfoProduct *prod)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(prod), OSINFO_PRODUCT_PROP_SHORT_ID);
}
const gchar *osinfo_product_get_name(OsinfoProduct *prod)
{
    return osinfo_entity_get_param_value(OSINFO_ENTITY(prod), OSINFO_PRODUCT_PROP_NAME);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
