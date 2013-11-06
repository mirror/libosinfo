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
#include <glib/gi18n-lib.h>

#include <osinfo/osinfo.h>

#include <gio/gio.h>

#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlreader.h>
#include "ignore-value.h"
#include "osinfo_install_script_private.h"
#include "osinfo_device_driver_private.h"

G_DEFINE_TYPE (OsinfoLoader, osinfo_loader, G_TYPE_OBJECT);

#define OSINFO_LOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_LOADER, OsinfoLoaderPrivate))

/**
 * SECTION:osinfo_loader
 * @short_description: An database loader
 * @see_also: #OsinfoDb
 *
 * #OsinfoLoader provides a way to populate an #OsinfoDb from
 * a set of XML documents.
 *
 */

struct _OsinfoLoaderPrivate
{
    OsinfoDb *db;
};

struct _OsinfoEntityKey
{
    const char *name;
    GType type;
};
typedef struct _OsinfoEntityKey OsinfoEntityKey;

static void
osinfo_loader_finalize (GObject *object)
{
    OsinfoLoader *loader = OSINFO_LOADER (object);

    g_object_unref(loader->priv->db);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_loader_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_loader_class_init (OsinfoLoaderClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

    g_klass->finalize = osinfo_loader_finalize;

    g_type_class_add_private (klass, sizeof (OsinfoLoaderPrivate));
}

static void
osinfo_loader_init (OsinfoLoader *loader)
{
    loader->priv = OSINFO_LOADER_GET_PRIVATE(loader);
    loader->priv->db = osinfo_db_new();
}

/** PUBLIC METHODS */

/**
 * osinfo_loader_new:
 *
 * Create a new database loader
 *
 * Returns: (transfer full): a loader object
 */
OsinfoLoader *osinfo_loader_new(void)
{
    return g_object_new(OSINFO_TYPE_LOADER, NULL);
}

#define OSINFO_ERROR(err, msg)                                          \
    g_set_error_literal((err), g_quark_from_static_string("libosinfo"), 0, (msg));

static gboolean error_is_set(GError **error)
{
    return ((error != NULL) && (*error != NULL));
}

static int
osinfo_loader_nodeset(const char *xpath,
                      xmlXPathContextPtr ctxt,
                      xmlNodePtr **list,
                      GError **err)
{
    xmlXPathObjectPtr obj;
    xmlNodePtr relnode;
    int ret;

    g_return_val_if_fail(ctxt != NULL, -1);
    g_return_val_if_fail(xpath != NULL, -1);

    if (list != NULL)
        *list = NULL;

    relnode = ctxt->node;
    obj = xmlXPathEval(BAD_CAST xpath, ctxt);
    ctxt->node = relnode;
    if (obj == NULL)
        return(0);
    if (obj->type != XPATH_NODESET) {
        g_set_error(err, g_quark_from_static_string("libosinfo"), 0,
                    _("Expected a nodeset in XPath query %s"), xpath);
        xmlXPathFreeObject(obj);
        return (-1);
    }
    if ((obj->nodesetval == NULL)  || (obj->nodesetval->nodeNr < 0)) {
        xmlXPathFreeObject(obj);
        return (0);
    }

    ret = obj->nodesetval->nodeNr;
    if (list != NULL && ret) {
        *list = g_new0(xmlNodePtr, ret);
        memcpy(*list, obj->nodesetval->nodeTab,
               ret * sizeof(xmlNodePtr));
    }
    xmlXPathFreeObject(obj);
    return (ret);
}

static gchar *
osinfo_loader_string(const char *xpath,
                     xmlXPathContextPtr ctxt,
                     GError **err)
{
    xmlXPathObjectPtr obj;
    xmlNodePtr relnode;
    gchar *ret;

    g_return_val_if_fail(ctxt != NULL, NULL);
    g_return_val_if_fail(xpath != NULL, NULL);

    relnode = ctxt->node;
    obj = xmlXPathEval(BAD_CAST xpath, ctxt);
    ctxt->node = relnode;
    if ((obj == NULL) || (obj->type != XPATH_STRING) ||
        (obj->stringval == NULL) || (obj->stringval[0] == 0)) {
        xmlXPathFreeObject(obj);
        return NULL;
    }
    ret = g_strdup((char *) obj->stringval);
    xmlXPathFreeObject(obj);

    return ret;
}

static gchar *
osinfo_loader_doc(const char *xpath,
                  xmlXPathContextPtr ctxt,
                  GError **err)
{
    xmlXPathObjectPtr obj;
    xmlNodePtr relnode;
    gchar *ret;
    xmlBufferPtr buf;

    g_return_val_if_fail(ctxt != NULL, NULL);
    g_return_val_if_fail(xpath != NULL, NULL);

    relnode = ctxt->node;
    obj = xmlXPathEval(BAD_CAST xpath, ctxt);
    ctxt->node = relnode;
    if ((obj == NULL) || (obj->type != XPATH_NODESET)) {
        xmlXPathFreeObject(obj);
        return NULL;
    }

    if (!(buf = xmlBufferCreate())) {
        xmlXPathFreeObject(obj);
        g_set_error(err, 0, 0, "%s",
                    "Cannot allocate buffer");
        return NULL;
    }
    if (xmlNodeDump(buf, NULL, obj->nodesetval->nodeTab[0], 0, 1) < 0) {
        xmlXPathFreeObject(obj);
        g_set_error(err, 0, 0, "%s",
                    "Cannot format stylesheet");
    }
    ret = g_strdup((char *)buf->content);

    xmlBufferFree(buf);
    xmlXPathFreeObject(obj);
    return ret;
}

static void osinfo_loader_entity(OsinfoLoader *loader,
                                 OsinfoEntity *entity,
                                 const OsinfoEntityKey *keys,
                                 xmlXPathContextPtr ctxt,
                                 xmlNodePtr root,
                                 GError **err)
{
    int i = 0;
    const gchar * const *langs = g_get_language_names ();

    /* Standard well-known keys first, allow single value only */
    for (i = 0 ; keys != NULL && keys[i].name != NULL; i++) {
        gchar *value_str = NULL;
        gchar *xpath = NULL;
        int j;

        /* We are guaranteed to have at least the default "C" locale and we
         * want to ignore that, hence the NULL check on index 'j + 1'.
         */
        if (keys[i].type == G_TYPE_STRING) {
            for (j = 0; langs[j + 1] != NULL; j++) {
                xpath = g_strdup_printf("string(./%s[lang('%s')])",
                                        keys[i].name, langs[j]);
                value_str = osinfo_loader_string(xpath, ctxt, err);
                g_free(xpath);
                xpath = NULL;
                if (error_is_set(err))
                    return;

                if (value_str != NULL)
                    break;
            }
        }

        switch (keys[i].type) {
            case G_TYPE_STRING:
                xpath = g_strdup_printf("string(./%s)", keys[i].name);
                if (value_str == NULL) {
                    value_str = osinfo_loader_string(xpath, ctxt, err);
                }
                break;
            default:
                g_warn_if_reached();
                break;
        }
        g_free(xpath);

        switch (keys[i].type) {
            case G_TYPE_STRING:
                if (value_str) {
                    osinfo_entity_set_param(entity, keys[i].name, value_str);
                    g_free(value_str);
                    value_str = NULL;
                }
                break;
            default:
                g_warn_if_reached();
                break;
        }
        g_warn_if_fail(value_str == NULL);
    }

    /* Then any site specific custom keys. x-... Can be repeated */
    xmlNodePtr *custom = NULL;
    int ncustom = osinfo_loader_nodeset("./*[substring(name(),1,2)='x-']", ctxt, &custom, err);
    if (error_is_set(err))
        return;

    for (i = 0 ; i < ncustom ; i++) {
        xmlNodePtr param = custom[i];

        if (!param->children ||
            param->children->type != XML_TEXT_NODE) {
            OSINFO_ERROR(err, _("Expected a text node attribute value"));
            goto cleanup;
        }

        osinfo_entity_add_param(entity,
                                (const char *)custom[i]->name,
                                (const char *)custom[i]->children->content);
    }

 cleanup:
    g_free(custom);
}

static OsinfoDatamap *osinfo_loader_get_datamap(OsinfoLoader *loader,
                                                 const gchar *id)
{
    OsinfoDatamap *datamap = osinfo_db_get_datamap(loader->priv->db, id);
    if (!datamap) {
        datamap = osinfo_datamap_new(id);
        osinfo_db_add_datamap(loader->priv->db, datamap);
        g_object_unref(datamap);
    }
    return datamap;
}

static OsinfoDevice *osinfo_loader_get_device(OsinfoLoader *loader,
                                              const gchar *id)
{
    OsinfoDevice *dev = osinfo_db_get_device(loader->priv->db, id);
    if (!dev) {
        dev = osinfo_device_new(id);
        osinfo_db_add_device(loader->priv->db, dev);
        g_object_unref(dev);
    }
    return dev;
}

static OsinfoOs *osinfo_loader_get_os(OsinfoLoader *loader,
                                      const gchar *id)
{
    OsinfoOs *os = osinfo_db_get_os(loader->priv->db, id);
    if (!os) {
        os = osinfo_os_new(id);
        osinfo_db_add_os(loader->priv->db, os);
        g_object_unref(os);
    }
    return os;
}

static OsinfoPlatform *osinfo_loader_get_platform(OsinfoLoader *loader,
                                                  const gchar *id)
{
    OsinfoPlatform *platform = osinfo_db_get_platform(loader->priv->db, id);
    if (!platform) {
        platform = osinfo_platform_new(id);
        osinfo_db_add_platform(loader->priv->db, platform);
        g_object_unref(platform);
    }
    return platform;
}

static OsinfoInstallScript *osinfo_loader_get_install_script(OsinfoLoader *loader,
                                                             const gchar *id)
{
    OsinfoInstallScript *script = osinfo_db_get_install_script(loader->priv->db, id);
    if (!script) {
        script = osinfo_install_script_new(id);
        osinfo_db_add_install_script(loader->priv->db, script);
        g_object_unref(script);
    }
    return script;
}

static void osinfo_loader_device(OsinfoLoader *loader,
                                 xmlXPathContextPtr ctxt,
                                 xmlNodePtr root,
                                 GError **err)
{
    gchar *id = (gchar *)xmlGetProp(root, BAD_CAST "id");
    const OsinfoEntityKey keys[] = {
        { OSINFO_DEVICE_PROP_VENDOR, G_TYPE_STRING },
        { OSINFO_DEVICE_PROP_VENDOR_ID, G_TYPE_STRING },
        { OSINFO_DEVICE_PROP_PRODUCT, G_TYPE_STRING },
        { OSINFO_DEVICE_PROP_PRODUCT_ID, G_TYPE_STRING },
        { OSINFO_DEVICE_PROP_SUBSYSTEM, G_TYPE_STRING },
        { OSINFO_DEVICE_PROP_BUS_TYPE, G_TYPE_STRING },
        { OSINFO_DEVICE_PROP_CLASS, G_TYPE_STRING },
        { OSINFO_DEVICE_PROP_NAME, G_TYPE_STRING },
        { NULL, G_TYPE_INVALID }
    };
    if (!id) {
        OSINFO_ERROR(err, _("Missing device id property"));
        return;
    }

    OsinfoDevice *device = osinfo_loader_get_device(loader, id);
    xmlFree(id);

    osinfo_loader_entity(loader, OSINFO_ENTITY(device), keys, ctxt, root, err);
}

static void osinfo_loader_device_link(OsinfoLoader *loader,
                                      OsinfoEntity *entity,
                                      const gchar *xpath,
                                      xmlXPathContextPtr ctxt,
                                      xmlNodePtr root,
                                      GError **err)
{
    xmlNodePtr *related = NULL;
    int nrelated = osinfo_loader_nodeset(xpath, ctxt, &related, err);
    int i;
    if (error_is_set(err))
        return;

    for (i = 0 ; i < nrelated ; i++) {
        const OsinfoEntityKey keys[] = {
            { OSINFO_DEVICELINK_PROP_DRIVER, G_TYPE_STRING },
            { NULL, G_TYPE_INVALID }
        };
        gchar *id = (gchar *)xmlGetProp(related[i], BAD_CAST "id");
        if (!id) {
            OSINFO_ERROR(err, _("Missing device link id property"));
            goto cleanup;
        }
        OsinfoDevice *dev = osinfo_loader_get_device(loader, id);
        xmlFree(id);

        OsinfoDeviceLink *devlink = NULL;
        if (OSINFO_IS_PLATFORM(entity)) {
            devlink = osinfo_platform_add_device(OSINFO_PLATFORM(entity), dev);
        } else if (OSINFO_IS_OS(entity)) {
            devlink = osinfo_os_add_device(OSINFO_OS(entity), dev);
        } else if (OSINFO_IS_DEPLOYMENT(entity)) {
            devlink = osinfo_deployment_add_device(OSINFO_DEPLOYMENT(entity), dev);
        }

        xmlNodePtr saved = ctxt->node;
        ctxt->node = related[i];
        osinfo_loader_entity(loader, OSINFO_ENTITY(devlink), keys, ctxt, root, err);
        ctxt->node = saved;
        if (error_is_set(err))
            goto cleanup;
    }

 cleanup:
    g_free(related);
}

static void osinfo_loader_product_relshp(OsinfoLoader *loader,
                                         OsinfoProduct *product,
                                         OsinfoProductRelationship relshp,
                                         const gchar *xpath,
                                         xmlXPathContextPtr ctxt,
                                         xmlNodePtr root,
                                         GError **err)
{
    xmlNodePtr *related = NULL;
    int nrelated = osinfo_loader_nodeset(xpath, ctxt, &related, err);
    int i;
    if (error_is_set(err))
        return;

    for (i = 0 ; i < nrelated ; i++) {
        gchar *id = (gchar *)xmlGetProp(related[i], BAD_CAST "id");
        if (!id) {
            OSINFO_ERROR(err, _("Missing product upgrades id property"));
            goto cleanup;
        }
        OsinfoProduct *relproduct;
        if (OSINFO_IS_PLATFORM(product))
            relproduct = OSINFO_PRODUCT(osinfo_loader_get_platform(loader, id));
        else
            relproduct = OSINFO_PRODUCT(osinfo_loader_get_os(loader, id));
        xmlFree(id);

        osinfo_product_add_related(product, relshp, relproduct);
    }

 cleanup:
    g_free(related);
}

static void osinfo_loader_product(OsinfoLoader *loader,
                                  OsinfoProduct *product,
                                  xmlXPathContextPtr ctxt,
                                  xmlNodePtr root,
                                  GError **err)
{
    const OsinfoEntityKey keys[] = {
        { OSINFO_PRODUCT_PROP_NAME, G_TYPE_STRING },
        { OSINFO_PRODUCT_PROP_VENDOR, G_TYPE_STRING },
        { OSINFO_PRODUCT_PROP_VERSION, G_TYPE_STRING },
        { OSINFO_PRODUCT_PROP_LOGO, G_TYPE_STRING },
        { OSINFO_PRODUCT_PROP_SHORT_ID, G_TYPE_STRING },
        { OSINFO_PRODUCT_PROP_RELEASE_DATE, G_TYPE_STRING },
        { OSINFO_PRODUCT_PROP_EOL_DATE, G_TYPE_STRING },
        { OSINFO_PRODUCT_PROP_CODENAME, G_TYPE_STRING },
        { NULL, G_TYPE_INVALID }
    };

    osinfo_loader_entity(loader, OSINFO_ENTITY(product), keys, ctxt, root, err);
    if (error_is_set(err))
        return;

    osinfo_loader_product_relshp(loader, product,
                                 OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM,
                                 "./derives-from",
                                 ctxt,
                                 root,
                                 err);
    if (error_is_set(err))
        return;

    osinfo_loader_product_relshp(loader, product,
                                 OSINFO_PRODUCT_RELATIONSHIP_CLONES,
                                 "./clones",
                                 ctxt,
                                 root,
                                 err);
    if (error_is_set(err))
        return;

    osinfo_loader_product_relshp(loader, product,
                                 OSINFO_PRODUCT_RELATIONSHIP_UPGRADES,
                                 "./upgrades",
                                 ctxt,
                                 root,
                                 err);
}

static void osinfo_loader_platform(OsinfoLoader *loader,
                                   xmlXPathContextPtr ctxt,
                                   xmlNodePtr root,
                                   GError **err)
{
    gchar *id = (gchar *)xmlGetProp(root, BAD_CAST "id");
    if (!id) {
        OSINFO_ERROR(err, _("Missing platform id property"));
        return;
    }

    OsinfoPlatform *platform = osinfo_loader_get_platform(loader, id);
    xmlFree(id);

    osinfo_loader_entity(loader, OSINFO_ENTITY(platform), NULL, ctxt, root, err);
    if (error_is_set(err))
        return;

    osinfo_loader_product(loader, OSINFO_PRODUCT(platform), ctxt, root, err);
    if (error_is_set(err))
        return;

    osinfo_loader_device_link(loader, OSINFO_ENTITY(platform),
                              "./devices/device", ctxt, root, err);
    if (error_is_set(err))
        return;
}

static void osinfo_loader_deployment(OsinfoLoader *loader,
                                     xmlXPathContextPtr ctxt,
                                     xmlNodePtr root,
                                     GError **err)
{
    gchar *id = (gchar *)xmlGetProp(root, BAD_CAST "id");
    if (!id) {
        OSINFO_ERROR(err, _("Missing deployment id property"));
        return;
    }

    gchar *osid = osinfo_loader_string("string(./os/@id)", ctxt, err);
    if (!osid && 0) {
        OSINFO_ERROR(err, _("Missing deployment os id property"));
        xmlFree(id);
        return;
    }
    OsinfoOs *os = osinfo_loader_get_os(loader, osid);
    g_free(osid);

    gchar *platformid = osinfo_loader_string("string(./platform/@id)", ctxt, err);
    if (!platformid) {
        OSINFO_ERROR(err, _("Missing deployment platform id property"));
        xmlFree(id);
        return;
    }
    OsinfoPlatform *platform = osinfo_loader_get_platform(loader, platformid);
    g_free(platformid);

    OsinfoDeployment *deployment = osinfo_deployment_new(id, os, platform);
    xmlFree(id);

    osinfo_loader_entity(loader, OSINFO_ENTITY(deployment), NULL, ctxt, root, err);
    if (error_is_set(err)) {
        g_object_unref(G_OBJECT(deployment));
        return;
    }

    osinfo_loader_device_link(loader, OSINFO_ENTITY(deployment),
                              "./devices/device", ctxt, root, err);
    if (error_is_set(err)) {
        g_object_unref(G_OBJECT(deployment));
        return;
    }

    osinfo_db_add_deployment(loader->priv->db, deployment);
}

static void osinfo_loader_datamap(OsinfoLoader *loader,
                                  xmlXPathContextPtr ctxt,
                                  xmlNodePtr root,
                                  GError **err)
{
    xmlNodePtr *nodes = NULL;
    guint i;
    int nnodes;

    gchar *id = (gchar *)xmlGetProp(root, BAD_CAST "id");

    if (!id) {
        OSINFO_ERROR(err, _("Missing os id property"));
        return;
    }

    OsinfoDatamap *map = osinfo_loader_get_datamap(loader, id);

    nnodes = osinfo_loader_nodeset("./entry", ctxt, &nodes, err);
    if (error_is_set(err))
        goto cleanup;

    for (i = 0 ; i < nnodes ; i++) {
        gchar *inval = (gchar *)xmlGetProp(nodes[i], BAD_CAST "inval");
        gchar *outval;

        if (inval == NULL)
            continue;
        outval = (gchar *)xmlGetProp(nodes[i], BAD_CAST "outval");
        osinfo_datamap_insert(map, inval, outval);

        xmlFree(inval);
        xmlFree(outval);
    }

cleanup:
    g_free(nodes);
    xmlFree(id);
}

static void osinfo_loader_install_config_params(OsinfoLoader *loader,
                                                OsinfoEntity *entity,
                                                const gchar *xpath,
                                                xmlXPathContextPtr ctxt,
                                                xmlNodePtr root,
                                                GError **err)
{
    xmlNodePtr *nodes = NULL;
    int nnodes = osinfo_loader_nodeset(xpath, ctxt, &nodes, err);
    int i;
    if (error_is_set(err))
        return;

    for (i = 0 ; i < nnodes ; i++) {
        gchar *name = (gchar *)xmlGetProp(nodes[i], BAD_CAST OSINFO_INSTALL_CONFIG_PARAM_PROP_NAME);
        gchar *policy = (gchar *)xmlGetProp(nodes[i], BAD_CAST OSINFO_INSTALL_CONFIG_PARAM_PROP_POLICY);
        gchar *mapid = (gchar *)xmlGetProp(nodes[i], BAD_CAST OSINFO_INSTALL_CONFIG_PARAM_PROP_DATAMAP);
        OsinfoInstallConfigParam *param = osinfo_install_config_param_new(name);
        osinfo_entity_set_param(OSINFO_ENTITY(param),
                                OSINFO_INSTALL_CONFIG_PARAM_PROP_POLICY,
                                policy);
        osinfo_install_script_add_config_param(OSINFO_INSTALL_SCRIPT(entity),
                                               param);
        if (mapid != NULL) {
            OsinfoDatamap *map;
            map = osinfo_loader_get_datamap(loader, mapid);
            if (map != NULL)
                osinfo_install_config_param_set_value_map(param, map);
        }

        xmlFree(mapid);
        xmlFree(name);
        xmlFree(policy);
    };

    g_free(nodes);
}

static OsinfoAvatarFormat *osinfo_loader_avatar_format(OsinfoLoader *loader,
                                                       xmlXPathContextPtr ctxt,
                                                       xmlNodePtr root,
                                                       GError **err)
{
    OsinfoAvatarFormat *avatar_format;
    const OsinfoEntityKey keys[] = {
        { OSINFO_AVATAR_FORMAT_PROP_MIME_TYPE, G_TYPE_STRING },
        { OSINFO_AVATAR_FORMAT_PROP_WIDTH, G_TYPE_STRING },
        { OSINFO_AVATAR_FORMAT_PROP_HEIGHT, G_TYPE_STRING },
        { OSINFO_AVATAR_FORMAT_PROP_ALPHA, G_TYPE_STRING },
        { NULL, G_TYPE_INVALID }
    };

    avatar_format = osinfo_avatar_format_new();

    osinfo_loader_entity(loader, OSINFO_ENTITY(avatar_format), keys, ctxt, root, err);
    if (error_is_set(err)) {
        g_object_unref (avatar_format);

        return NULL;
    }

    return avatar_format;
}

static void osinfo_loader_install_script(OsinfoLoader *loader,
                                         xmlXPathContextPtr ctxt,
                                         xmlNodePtr root,
                                         GError **err)
{
    gchar *id = (gchar *)xmlGetProp(root, BAD_CAST "id");
    const OsinfoEntityKey keys[] = {
        { OSINFO_INSTALL_SCRIPT_PROP_PROFILE, G_TYPE_STRING },
        { OSINFO_INSTALL_SCRIPT_PROP_PRODUCT_KEY_FORMAT, G_TYPE_STRING },
        { OSINFO_INSTALL_SCRIPT_PROP_PATH_FORMAT, G_TYPE_STRING },
        { OSINFO_INSTALL_SCRIPT_PROP_EXPECTED_FILENAME, G_TYPE_STRING },
        { OSINFO_INSTALL_SCRIPT_PROP_CAN_PRE_INSTALL_DRIVERS, G_TYPE_STRING },
        { OSINFO_INSTALL_SCRIPT_PROP_CAN_POST_INSTALL_DRIVERS, G_TYPE_STRING },
        { OSINFO_INSTALL_SCRIPT_PROP_PRE_INSTALL_DRIVERS_SIGNING_REQ, G_TYPE_STRING },
        { OSINFO_INSTALL_SCRIPT_PROP_POST_INSTALL_DRIVERS_SIGNING_REQ, G_TYPE_STRING },
        { NULL, G_TYPE_INVALID }
    };
    gchar *value = NULL;
    xmlNodePtr *nodes = NULL;
    int nnodes;

    if (!id) {
        OSINFO_ERROR(err, _("Missing install script id property"));
        return;
    }

    OsinfoInstallScript *installScript = osinfo_loader_get_install_script(loader,
                                                                          id);
    xmlFree(id);

    osinfo_loader_entity(loader, OSINFO_ENTITY(installScript), keys, ctxt, root, err);
    if (error_is_set(err))
        goto error;

    value = osinfo_loader_doc("./template/*[1]", ctxt, err);
    if (error_is_set(err))
        goto error;
    if (value)
        osinfo_entity_set_param(OSINFO_ENTITY(installScript),
                                OSINFO_INSTALL_SCRIPT_PROP_TEMPLATE_DATA,
                                value);
    g_free(value);

    value = osinfo_loader_string("./template/@uri", ctxt, err);
    if (error_is_set(err))
        goto error;
    if (value)
        osinfo_entity_set_param(OSINFO_ENTITY(installScript),
                                OSINFO_INSTALL_SCRIPT_PROP_TEMPLATE_URI,
                                value);
    g_free(value);

    osinfo_loader_install_config_params(loader,
                                        OSINFO_ENTITY(installScript),
                                        "./config/*",
                                        ctxt,
                                        root,
                                        err);

    nnodes = osinfo_loader_nodeset("./avatar-format", ctxt, &nodes, err);
    if (error_is_set(err))
        goto error;

    if (nnodes > 0) {
        OsinfoAvatarFormat *avatar_format;

        xmlNodePtr saved = ctxt->node;
        ctxt->node = nodes[0];
        avatar_format = osinfo_loader_avatar_format(loader, ctxt, root, err);
        ctxt->node = saved;
        if (error_is_set(err))
            goto error;

        osinfo_install_script_set_avatar_format(installScript, avatar_format);
    }
    g_free(nodes);

    osinfo_db_add_install_script(loader->priv->db, installScript);

    return;

 error:
    g_free(nodes);
    g_free(value);
    g_object_unref(installScript);
}

static OsinfoMedia *osinfo_loader_media (OsinfoLoader *loader,
                                         xmlXPathContextPtr ctxt,
                                         xmlNodePtr root,
                                         const gchar *id,
                                         GError **err)
{
    xmlNodePtr *nodes = NULL;
    guint i;

    gchar *arch = (gchar *)xmlGetProp(root, BAD_CAST "arch");
    xmlChar *live = xmlGetProp(root, BAD_CAST OSINFO_MEDIA_PROP_LIVE);
    xmlChar *installer = xmlGetProp(root, BAD_CAST OSINFO_MEDIA_PROP_INSTALLER);
    xmlChar *installer_reboots =
            xmlGetProp(root, BAD_CAST OSINFO_MEDIA_PROP_INSTALLER_REBOOTS);
    const OsinfoEntityKey keys[] = {
        { OSINFO_MEDIA_PROP_URL, G_TYPE_STRING },
        { OSINFO_MEDIA_PROP_KERNEL, G_TYPE_STRING },
        { OSINFO_MEDIA_PROP_INITRD, G_TYPE_STRING },
        { NULL, G_TYPE_INVALID }
    };

    OsinfoMedia *media = osinfo_media_new(id, arch);
    xmlFree(arch);

    osinfo_loader_entity(loader, OSINFO_ENTITY(media), keys, ctxt, root, err);
    if (live) {
        osinfo_entity_set_param(OSINFO_ENTITY(media),
                                OSINFO_MEDIA_PROP_LIVE,
                                (gchar *)live);
        xmlFree(live);
    }

    if (installer) {
        osinfo_entity_set_param(OSINFO_ENTITY(media),
                                OSINFO_MEDIA_PROP_INSTALLER,
                                (gchar *)installer);
        xmlFree(installer);
    }

    if (installer_reboots) {
        osinfo_entity_set_param(OSINFO_ENTITY(media),
                                OSINFO_MEDIA_PROP_INSTALLER_REBOOTS,
                                (gchar *)installer_reboots);
        xmlFree(installer_reboots);
    }

    gint nnodes = osinfo_loader_nodeset("./iso/*", ctxt, &nodes, err);
    if (error_is_set(err)) {
        g_object_unref(media);
        return NULL;
    }

    for (i = 0 ; i < nnodes ; i++) {
        if (!nodes[i]->children ||
            nodes[i]->children->type != XML_TEXT_NODE ||
            (strcmp((const gchar *)nodes[i]->name,
                    OSINFO_MEDIA_PROP_VOLUME_ID) != 0 &&
             strcmp((const gchar *)nodes[i]->name,
                    OSINFO_MEDIA_PROP_SYSTEM_ID) != 0 &&
             strcmp((const gchar *)nodes[i]->name,
                    OSINFO_MEDIA_PROP_PUBLISHER_ID) != 0 &&
             strcmp((const gchar *)nodes[i]->name,
                    OSINFO_MEDIA_PROP_APPLICATION_ID) != 0 &&
             strcmp((const gchar *)nodes[i]->name,
                    OSINFO_MEDIA_PROP_LANG) != 0))
            continue;

        if (strcmp((const gchar *)nodes[i]->name,
                   OSINFO_MEDIA_PROP_LANG) == 0) {
            gchar *regex = (gchar *)xmlGetProp(nodes[i], BAD_CAST "regex");
            if (g_strcmp0(regex, "true") == 0) {
                gchar *datamap;
                osinfo_entity_set_param(OSINFO_ENTITY(media),
                                        OSINFO_MEDIA_PROP_LANG_REGEX,
                                        (const gchar *)nodes[i]->children->content);
                datamap = (gchar *)xmlGetProp(nodes[i], BAD_CAST OSINFO_MEDIA_PROP_LANG_MAP);
                if (datamap != NULL)
                    osinfo_entity_set_param(OSINFO_ENTITY(media),
                                            OSINFO_MEDIA_PROP_LANG_MAP,
                                            datamap);
                xmlFree(datamap);
            } else {
                osinfo_entity_add_param(OSINFO_ENTITY(media),
                                        OSINFO_MEDIA_PROP_LANG,
                                        (const gchar *)nodes[i]->children->content);
            }
            xmlFree(regex);
        } else {
            osinfo_entity_set_param(OSINFO_ENTITY(media),
                                    (const gchar *)nodes[i]->name,
                                    (const gchar *)nodes[i]->children->content);
        }
    }

    g_free(nodes);

    return media;
}

static OsinfoTree *osinfo_loader_tree (OsinfoLoader *loader,
                                         xmlXPathContextPtr ctxt,
                                         xmlNodePtr root,
                                         const gchar *id,
                                         GError **err)
{
    xmlNodePtr *nodes = NULL;
    guint i;

    gchar *arch = (gchar *)xmlGetProp(root, BAD_CAST "arch");
    const OsinfoEntityKey keys[] = {
        { OSINFO_TREE_PROP_URL, G_TYPE_STRING },
        { OSINFO_TREE_PROP_KERNEL, G_TYPE_STRING },
        { OSINFO_TREE_PROP_INITRD, G_TYPE_STRING },
        { OSINFO_TREE_PROP_BOOT_ISO, G_TYPE_STRING },
        { NULL, G_TYPE_INVALID }
    };

    OsinfoTree *tree = osinfo_tree_new(id, arch);
    xmlFree(arch);

    osinfo_loader_entity(loader, OSINFO_ENTITY(tree), keys, ctxt, root, err);

    gint nnodes = osinfo_loader_nodeset("./treeinfo/*", ctxt, &nodes, err);
    if (error_is_set(err)) {
        g_object_unref(G_OBJECT(tree));
        return NULL;
    }

    for (i = 0 ; i < nnodes ; i++) {
        if (!nodes[i]->children ||
            nodes[i]->children->type != XML_TEXT_NODE)
            continue;

        if (strcmp((const gchar *)nodes[i]->name,
                   OSINFO_TREE_PROP_TREEINFO_FAMILY + sizeof("treeinfo-")) == 0)
            osinfo_entity_set_param(OSINFO_ENTITY(tree),
                                    OSINFO_TREE_PROP_TREEINFO_FAMILY,
                                    (const gchar *)nodes[i]->children->content);
        else if (strcmp((const gchar *)nodes[i]->name,
                        OSINFO_TREE_PROP_TREEINFO_VARIANT + sizeof("treeinfo-")) == 0)
            osinfo_entity_set_param(OSINFO_ENTITY(tree),
                                    OSINFO_TREE_PROP_TREEINFO_VARIANT,
                                    (const gchar *)nodes[i]->children->content);
        else if (strcmp((const gchar *)nodes[i]->name,
                        OSINFO_TREE_PROP_TREEINFO_VERSION + sizeof("treeinfo-")) == 0)
            osinfo_entity_set_param(OSINFO_ENTITY(tree),
                                    OSINFO_TREE_PROP_TREEINFO_VERSION,
                                    (const gchar *)nodes[i]->children->content);
        else if (strcmp((const gchar *)nodes[i]->name,
                        OSINFO_TREE_PROP_TREEINFO_ARCH + sizeof("treeinfo-")) == 0)
            osinfo_entity_set_param(OSINFO_ENTITY(tree),
                                    OSINFO_TREE_PROP_TREEINFO_ARCH,
                                    (const gchar *)nodes[i]->children->content);
    }

    g_free(nodes);

    return tree;
}

static OsinfoResources *osinfo_loader_resources(OsinfoLoader *loader,
                                                xmlXPathContextPtr ctxt,
                                                xmlNodePtr root,
                                                const gchar *id,
                                                const gchar *name,
                                                GError **err)
{
    xmlNodePtr *nodes = NULL;
    OsinfoResources *resources = NULL;
    guint i;

    gchar *arch = (gchar *)xmlGetProp(root, BAD_CAST "arch");
    gchar *node_path = g_strjoin("/", ".", name, "*", NULL);
    gint nnodes = osinfo_loader_nodeset(node_path, ctxt, &nodes, err);
    g_free(node_path);
    if (error_is_set(err) || nnodes < 1)
        goto EXIT;

    resources = osinfo_resources_new(id, arch);

    for (i = 0 ; i < nnodes ; i++) {
        if (!nodes[i]->children ||
            nodes[i]->children->type != XML_TEXT_NODE ||
            (strcmp((const gchar *)nodes[i]->name,
                    OSINFO_RESOURCES_PROP_CPU) != 0 &&
             strcmp((const gchar *)nodes[i]->name,
                    OSINFO_RESOURCES_PROP_N_CPUS) != 0 &&
             strcmp((const gchar *)nodes[i]->name,
                    OSINFO_RESOURCES_PROP_RAM) != 0 &&
             strcmp((const gchar *)nodes[i]->name,
                    OSINFO_RESOURCES_PROP_STORAGE) != 0))
            continue;

        osinfo_entity_set_param(OSINFO_ENTITY(resources),
                                (const gchar *)nodes[i]->name,
                                (const gchar *)nodes[i]->children->content);
    }

EXIT:
    g_free(nodes);
    xmlFree(arch);

    return resources;
}

static void osinfo_loader_resources_list(OsinfoLoader *loader,
                                         xmlXPathContextPtr ctxt,
                                         xmlNodePtr root,
                                         const gchar *id,
                                         OsinfoOs *os,
                                         GError **err)
{
    OsinfoResources *resources;

    resources = osinfo_loader_resources(loader, ctxt, root, id, "minimum", err);
    if (error_is_set(err))
        goto EXIT;

    if (resources != NULL)
        osinfo_os_add_minimum_resources(os, resources);

    g_clear_object(&resources);
    resources = osinfo_loader_resources(loader, ctxt, root, id, "recommended", err);
    if (error_is_set(err))
        goto EXIT;

    if (resources != NULL)
        osinfo_os_add_recommended_resources(os, resources);

EXIT:
    g_clear_object(&resources);
}

static OsinfoDeviceDriver *osinfo_loader_driver(OsinfoLoader *loader,
                                                xmlXPathContextPtr ctxt,
                                                xmlNodePtr root,
                                                const gchar *id,
                                                GError **err)
{
    xmlNodePtr *nodes = NULL;
    guint i;

    xmlChar *arch = xmlGetProp(root, BAD_CAST OSINFO_DEVICE_DRIVER_PROP_ARCHITECTURE);
    xmlChar *location = xmlGetProp(root, BAD_CAST OSINFO_DEVICE_DRIVER_PROP_LOCATION);
    xmlChar *preinst = xmlGetProp(root, BAD_CAST OSINFO_DEVICE_DRIVER_PROP_PRE_INSTALLABLE);
    xmlChar *is_signed = xmlGetProp(root, BAD_CAST OSINFO_DEVICE_DRIVER_PROP_SIGNED);

    OsinfoDeviceDriver *driver = osinfo_device_driver_new(id);

    if (arch) {
        osinfo_entity_set_param(OSINFO_ENTITY(driver),
                                OSINFO_DEVICE_DRIVER_PROP_ARCHITECTURE,
                                (gchar *)arch);
        xmlFree(arch);
    }

    if (location) {
        osinfo_entity_set_param(OSINFO_ENTITY(driver),
                                OSINFO_DEVICE_DRIVER_PROP_LOCATION,
                                (gchar *)location);
        xmlFree(location);
    }

    if (preinst) {
        osinfo_entity_set_param(OSINFO_ENTITY(driver),
                                OSINFO_DEVICE_DRIVER_PROP_PRE_INSTALLABLE,
                                (gchar *)preinst);
        xmlFree(preinst);
    }

    if (is_signed) {
        osinfo_entity_set_param(OSINFO_ENTITY(driver),
                                OSINFO_DEVICE_DRIVER_PROP_SIGNED,
                                (gchar *)is_signed);
        xmlFree(is_signed);
    }

    gint nnodes = osinfo_loader_nodeset("./*", ctxt, &nodes, err);
    if (error_is_set(err)) {
        g_object_unref(G_OBJECT(driver));
        return NULL;
    }

    for (i = 0 ; i < nnodes ; i++) {
        if (nodes[i]->children &&
            nodes[i]->children->type == XML_TEXT_NODE &&
            (strcmp((const gchar *)nodes[i]->name,
                    OSINFO_DEVICE_DRIVER_PROP_FILE) == 0)) {
            osinfo_entity_add_param(OSINFO_ENTITY(driver),
                                    (const gchar *)nodes[i]->name,
                                    (const gchar *)nodes[i]->children->content);
        } else if (strcmp((const gchar *)nodes[i]->name,
                          OSINFO_DEVICE_DRIVER_PROP_DEVICE) == 0) {
            xmlChar *device_id = xmlGetProp(nodes[i], BAD_CAST "id");
            OsinfoDevice *device = osinfo_loader_get_device(loader,
                                                            (gchar *)device_id);
            xmlFree(device_id);

            osinfo_device_driver_add_device(driver, device);
        }
    }

    g_free(nodes);

    return driver;
}


static void osinfo_loader_os(OsinfoLoader *loader,
                             xmlXPathContextPtr ctxt,
                             xmlNodePtr root,
                             GError **err)
{
    xmlNodePtr *nodes = NULL;
    guint i;
    int nnodes;

    gchar *id = (gchar *)xmlGetProp(root, BAD_CAST "id");
    const OsinfoEntityKey keys[] = {
        { OSINFO_OS_PROP_FAMILY, G_TYPE_STRING },
        { OSINFO_OS_PROP_DISTRO, G_TYPE_STRING },
        { NULL, G_TYPE_INVALID }
    };

    if (!id) {
        OSINFO_ERROR(err, _("Missing os id property"));
        return;
    }

    OsinfoOs *os = osinfo_loader_get_os(loader, id);

    osinfo_loader_entity(loader, OSINFO_ENTITY(os), keys, ctxt, root, err);
    if (error_is_set(err))
        goto cleanup;

    osinfo_loader_product(loader, OSINFO_PRODUCT(os), ctxt, root, err);
    if (error_is_set(err))
        goto cleanup;

    osinfo_loader_device_link(loader, OSINFO_ENTITY(os),
                              "./devices/device", ctxt, root, err);
    if (error_is_set(err))
        goto cleanup;

    nnodes = osinfo_loader_nodeset("./media", ctxt, &nodes, err);
    if (error_is_set(err))
        goto cleanup;

    for (i = 0 ; i < nnodes ; i++) {
        xmlNodePtr saved = ctxt->node;
        ctxt->node = nodes[i];
        gchar *media_id = g_strdup_printf ("%s:%u", id, i);
        OsinfoMedia *media = osinfo_loader_media(loader, ctxt, nodes[i], media_id, err);
        g_free (media_id);
        ctxt->node = saved;
        if (error_is_set(err))
            goto cleanup;

        osinfo_os_add_media (os, media);
        g_object_unref (media);
    }

    g_free(nodes);

    nnodes = osinfo_loader_nodeset("./tree", ctxt, &nodes, err);
    if (error_is_set(err))
        goto cleanup;

    for (i = 0 ; i < nnodes ; i++) {
        xmlNodePtr saved = ctxt->node;
        ctxt->node = nodes[i];
        gchar *tree_id = g_strdup_printf ("%s:%u", id, i);
        OsinfoTree *tree = osinfo_loader_tree(loader, ctxt, nodes[i], tree_id, err);
        g_free (tree_id);
        ctxt->node = saved;
        if (error_is_set(err))
            goto cleanup;

        osinfo_os_add_tree (os, tree);
        g_object_unref(G_OBJECT(tree));
    }

    g_free(nodes);

    nnodes = osinfo_loader_nodeset("./resources", ctxt, &nodes, err);
    if (error_is_set(err))
        goto cleanup;

    for (i = 0 ; i < nnodes ; i++) {
        xmlNodePtr saved = ctxt->node;
        ctxt->node = nodes[i];
        gchar *resources_id = g_strdup_printf ("%s:%u", id, i);

        osinfo_loader_resources_list(loader,
                                     ctxt,
                                     nodes[i],
                                     resources_id,
                                     os,
                                     err);
        g_free (resources_id);
        ctxt->node = saved;
    }

    g_free(nodes);


    nnodes = osinfo_loader_nodeset("./installer/script", ctxt, &nodes, err);
    if (error_is_set(err))
        goto cleanup;

    for (i = 0 ; i < nnodes ; i++) {
        gchar *scriptid = (gchar *)xmlGetProp(nodes[i], BAD_CAST "id");
        if (!scriptid) {
            OSINFO_ERROR(err, _("Missing OS install script property"));
            goto cleanup;
        }
        OsinfoInstallScript *script;
        script = osinfo_loader_get_install_script(loader, scriptid);
        xmlFree(scriptid);

        osinfo_os_add_install_script(os, script);
    }

    g_free(nodes);

    nnodes = osinfo_loader_nodeset("./driver", ctxt, &nodes, err);
    if (error_is_set(err))
        goto cleanup;

    for (i = 0 ; i < nnodes ; i++) {
        xmlNodePtr saved = ctxt->node;
        ctxt->node = nodes[i];
        gchar *driver_id = g_strdup_printf("%s:%u", id, i);
        OsinfoDeviceDriver *driver= osinfo_loader_driver(loader,
                                                         ctxt,
                                                         nodes[i],
                                                         driver_id,
                                                         err);
        g_free (driver_id);
        ctxt->node = saved;
        if (error_is_set(err))
            break;

        osinfo_os_add_device_driver(os, driver);
        g_object_unref(driver);
    }

cleanup:
    g_free(nodes);
    xmlFree(id);
}

static void osinfo_loader_root(OsinfoLoader *loader,
                               xmlXPathContextPtr ctxt,
                               xmlNodePtr root,
                               GError **err)
{
    /*
     * File assumed to contain data in XML format. All data
     * is within <libosinfo>...</libosinfo>. The following steps are taken
     * to process the data within the file:
     *
     * Advance till we are at opening <libosinfo> tag.
     * While true:
     *   Advance tag
     *   If closing libosinfo tag, break
     *   If non element tag, continue
     *   If element tag, and element is not os, platform, device,
     *   datamap, deployment or install-script, error
     *   Else, switch on tag type and handle reading in data
     * After loop, return success if no error
     * If there was an error, clean up lib data acquired so far
     */
    xmlNodePtr *oss = NULL;
    xmlNodePtr *devices = NULL;
    xmlNodePtr *platforms = NULL;
    xmlNodePtr *deployments = NULL;
    xmlNodePtr *installScripts = NULL;
    xmlNodePtr *dataMaps = NULL;
    int i;
    int ndeployment;
    int nos;
    int ndevice;
    int nplatform;
    int ninstallScript;
    int ndataMaps;

    if (!xmlStrEqual(root->name, BAD_CAST "libosinfo")) {
        OSINFO_ERROR(err, _("Incorrect root element"));
        return;
    }

    ndevice = osinfo_loader_nodeset("./device", ctxt, &devices, err);
    if (error_is_set(err))
        goto cleanup;

    for (i = 0 ; i < ndevice ; i++) {
        xmlNodePtr saved = ctxt->node;
        ctxt->node = devices[i];
        osinfo_loader_device(loader, ctxt, devices[i], err);
        ctxt->node = saved;
        if (error_is_set(err))
            goto cleanup;
    }

    nplatform = osinfo_loader_nodeset("./platform", ctxt, &platforms, err);
    if (error_is_set(err))
        goto cleanup;

    for (i = 0 ; i < nplatform ; i++) {
        xmlNodePtr saved = ctxt->node;
        ctxt->node = platforms[i];
        osinfo_loader_platform(loader, ctxt, platforms[i], err);
        ctxt->node = saved;
        if (error_is_set(err))
            goto cleanup;
    }

    nos = osinfo_loader_nodeset("./os", ctxt, &oss, err);
    if (error_is_set(err))
        goto cleanup;

    for (i = 0 ; i < nos ; i++) {
        xmlNodePtr saved = ctxt->node;
        ctxt->node = oss[i];
        osinfo_loader_os(loader, ctxt, oss[i], err);
        ctxt->node = saved;
        if (error_is_set(err))
            goto cleanup;
    }

    ndeployment = osinfo_loader_nodeset("./deployment", ctxt, &deployments, err);
    if (error_is_set(err))
        goto cleanup;

    for (i = 0 ; i < ndeployment ; i++) {
        xmlNodePtr saved = ctxt->node;
        ctxt->node = deployments[i];
        osinfo_loader_deployment(loader, ctxt, deployments[i], err);
        ctxt->node = saved;
        if (error_is_set(err))
            goto cleanup;
    }

    ninstallScript = osinfo_loader_nodeset("./install-script", ctxt, &installScripts, err);
    if (error_is_set(err))
        goto cleanup;

    for (i = 0 ; i < ninstallScript ; i++) {
        xmlNodePtr saved = ctxt->node;
        ctxt->node = installScripts[i];
        osinfo_loader_install_script(loader, ctxt, installScripts[i], err);
        ctxt->node = saved;
        if (error_is_set(err))
            goto cleanup;
    }

    ndataMaps = osinfo_loader_nodeset("./datamap", ctxt, &dataMaps, err);
    if (error_is_set(err))
        goto cleanup;

    for (i = 0 ; i < ndataMaps ; i++) {
        xmlNodePtr saved = ctxt->node;
        ctxt->node = dataMaps[i];
        osinfo_loader_datamap(loader, ctxt, dataMaps[i], err);
        ctxt->node = saved;
        if (error_is_set(err))
            goto cleanup;
    }

 cleanup:
    g_free(dataMaps);
    g_free(installScripts);
    g_free(deployments);
    g_free(platforms);
    g_free(oss);
    g_free(devices);
}

static void
catchXMLError(void *ctx, const char *msg ATTRIBUTE_UNUSED, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;

    if (ctxt && ctxt->_private) {
        GError **err = ctxt->_private;
        if (!error_is_set(err)) {
            gchar *xmlmsg = g_strdup_printf("at line %d: %s",
                                            ctxt->lastError.line,
                                            ctxt->lastError.message);
            OSINFO_ERROR(ctxt->_private, xmlmsg);
            g_free(xmlmsg);
        }
    }
}

static void osinfo_loader_process_xml(OsinfoLoader *loader,
                                      const gchar *xmlStr,
                                      const gchar *src,
                                      GError **err)
{
    xmlParserCtxtPtr pctxt;
    xmlXPathContextPtr ctxt = NULL;
    xmlDocPtr xml = NULL;
    xmlNodePtr root;

    /* Set up a parser context so we can catch the details of XML errors. */
    pctxt = xmlNewParserCtxt();
    if (!pctxt || !pctxt->sax) {
        OSINFO_ERROR(err, _("Unable to construct parser context"));
        goto cleanup;
    }

    pctxt->_private = err;
    pctxt->sax->error = catchXMLError;

    xml = xmlCtxtReadDoc(pctxt, BAD_CAST xmlStr, src, NULL,
                         XML_PARSE_NOENT | XML_PARSE_NONET |
                         XML_PARSE_NOWARNING);
    if (!xml)
        goto cleanup;

    /* catchXMLError may be called for non-fatal errors. If that happens,
     * xml will be non-NULL but err may be set. This will cause warnings if
     * we try to set err later on, so clear it first.
     */
    g_clear_error(err);

    root = xmlDocGetRootElement(xml);

    if (!root) {
        OSINFO_ERROR(err, _("Missing root XML element"));
        goto cleanup;
    }

    ctxt = xmlXPathNewContext(xml);
    if (!ctxt)
        goto cleanup;

    ctxt->node = root;

    osinfo_loader_root(loader, ctxt, root, err);

 cleanup:
    xmlXPathFreeContext(ctxt);
    xmlFreeDoc(xml);
    xmlFreeParserCtxt(pctxt);
}

static void
osinfo_loader_process_file_reg_ids(OsinfoLoader *loader,
                                   GFile *file,
                                   GFileInfo *info,
                                   gboolean withSubsys,
                                   const char *baseURI,
                                   const char *busType,
                                   GError **err)
{
    GFileInputStream *is = g_file_read(file, NULL, err);
    if (error_is_set(err))
        return;

    GDataInputStream *dis = g_data_input_stream_new(G_INPUT_STREAM(is));

    gchar *data;
    gsize datalen;

    gchar *vendor_buf = NULL;
    gchar *vendor = NULL;
    gchar *vendor_id = NULL;
    gchar *device_buf = NULL;
    gchar *device = NULL;
    gchar *device_id = NULL;
    gchar *subsystem = NULL;
    gchar *subvendor_id = NULL;
    gchar *subdevice_id = NULL;

    while ((data = g_data_input_stream_read_line(dis, &datalen, NULL, err)) != NULL) {
        char *tmp = data;
        gsize offset = 0;

        if (data[0] == '#')
            goto done;

#define GOT_TAB() \
        (offset < datalen && tmp[offset] == '\t')

#define GOT_ID()                             \
        (((offset + 5) < datalen) &&         \
         g_ascii_isxdigit(tmp[offset])    && \
         g_ascii_isxdigit(tmp[offset+1])  && \
         g_ascii_isxdigit(tmp[offset+2])  && \
         g_ascii_isxdigit(tmp[offset+3])  && \
         g_ascii_isspace(tmp[offset+4]))

#define FREE_BUF(var) \
        g_free(var);  \
        (var) = NULL

#define SAVE_BUF(var) \
        (var) = data; \
        data = NULL

#define WANT_ID(var)                            \
        if (GOT_ID()) {                         \
            (var) = tmp+offset;                 \
            offset += 4;                        \
            tmp[offset] = '\0';                 \
            offset += 1;                        \
        } else {                                \
            goto done;                          \
        }
#define WANT_REST(var)                          \
        (var) = tmp+offset

        if (GOT_TAB()) {
            offset++;
            if (!vendor_id)
                goto done;
            if (GOT_TAB()) {
                offset++;
                WANT_ID(subvendor_id);
                WANT_ID(subdevice_id);
                WANT_REST(subsystem);
                ignore_value(subvendor_id);
                ignore_value(subdevice_id);
                ignore_value(subsystem);
            } else {
                FREE_BUF(device_buf);
                WANT_ID(device_id);
                WANT_REST(device);
                SAVE_BUF(device_buf);

                gchar *id = g_strdup_printf("%s/%s/%s",
                                            baseURI, vendor_id, device_id);

                OsinfoDevice *dev = osinfo_loader_get_device(loader, id);
                osinfo_entity_set_param(OSINFO_ENTITY(dev),
                                        OSINFO_DEVICE_PROP_VENDOR_ID,
                                        vendor_id);
                osinfo_entity_set_param(OSINFO_ENTITY(dev),
                                        OSINFO_DEVICE_PROP_VENDOR,
                                        vendor);
                osinfo_entity_set_param(OSINFO_ENTITY(dev),
                                        OSINFO_DEVICE_PROP_PRODUCT_ID,
                                        device_id);
                osinfo_entity_set_param(OSINFO_ENTITY(dev),
                                        OSINFO_DEVICE_PROP_PRODUCT,
                                        device);
                osinfo_entity_set_param(OSINFO_ENTITY(dev),
                                        OSINFO_DEVICE_PROP_BUS_TYPE,
                                        busType);
                g_free(id);
            }
        } else {
            FREE_BUF(vendor_buf);
            WANT_ID(vendor_id);
            WANT_REST(vendor);
            SAVE_BUF(vendor_buf);
        }

    done:
        g_free(data);
        if (error_is_set(err))
            break;
    }

    FREE_BUF(device_buf);
    FREE_BUF(vendor_buf);
    g_object_unref(dis);
    g_object_unref(is);
}

static void
osinfo_loader_process_file_reg_usb(OsinfoLoader *loader,
                                   GFile *file,
                                   GFileInfo *info,
                                   GError **err)
{
    osinfo_loader_process_file_reg_ids(loader,
                                       file,
                                       info,
                                       FALSE,
                                       "http://www.linux-usb.org/usb.ids",
                                       "usb",
                                       err);
}

static void
osinfo_loader_process_file_reg_pci(OsinfoLoader *loader,
                                   GFile *file,
                                   GFileInfo *info,
                                   GError **err)
{
    osinfo_loader_process_file_reg_ids(loader,
                                       file,
                                       info,
                                       TRUE,
                                       "http://pciids.sourceforge.net/v2.2/pci.ids",
                                       "pci",
                                       err);
}

static void
osinfo_loader_process_file(OsinfoLoader *loader,
                           GFile *file,
                           gboolean ignoreMissing,
                           GError **err);

static void
osinfo_loader_process_file_reg_xml(OsinfoLoader *loader,
                                   GFile *file,
                                   GFileInfo *info,
                                   GError **err)
{
    gchar *xml = NULL;
    gsize xmlLen;
    g_file_load_contents(file, NULL, &xml, &xmlLen, NULL, err);
    if (error_is_set(err))
        return;

    gchar *uri = g_file_get_uri(file);
    osinfo_loader_process_xml(loader,
                              xml,
                              uri,
                              err);
    g_free(uri);
    g_free(xml);
}

static void
osinfo_loader_process_file_dir(OsinfoLoader *loader,
                               GFile *file,
                               GFileInfo *info,
                               GError **err)
{
    GFileEnumerator *ents = g_file_enumerate_children(file,
                                                      "standard::*",
                                                      G_FILE_QUERY_INFO_NONE,
                                                      NULL,
                                                      err);
    if (error_is_set(err))
        return;

    GFileInfo *child;
    while ((child = g_file_enumerator_next_file(ents, NULL, err)) != NULL) {
        const gchar *name = g_file_info_get_name(child);
        GFile *ent = g_file_get_child(file, name);

        osinfo_loader_process_file(loader, ent, FALSE, err);

        g_object_unref(ent);
        g_object_unref(child);

        if (error_is_set(err))
            break;
    }

    g_object_unref(ents);
}

static void
osinfo_loader_process_file(OsinfoLoader *loader,
                           GFile *file,
                           gboolean ignoreMissing,
                           GError **err)
{
    GError *error = NULL;
    GFileInfo *info = g_file_query_info(file,
                                        "standard::*",
                                        G_FILE_QUERY_INFO_NONE,
                                        NULL,
                                        &error);
    const char *name;

    if (error) {
        if (ignoreMissing &&
            (error->code == G_IO_ERROR_NOT_FOUND)) {
            g_error_free(error);
            return;
        }
        g_propagate_error(err, error);
        return;
    }

    name = g_file_info_get_name(info);

    GFileType type = g_file_info_get_attribute_uint32(info,
                                                      G_FILE_ATTRIBUTE_STANDARD_TYPE);

    switch (type) {
    case G_FILE_TYPE_REGULAR:
        if (g_str_has_suffix(name, ".xml"))
            osinfo_loader_process_file_reg_xml(loader, file, info, &error);
        else if (strcmp(name, "usb.ids") == 0)
            osinfo_loader_process_file_reg_usb(loader, file, info, &error);
        else if (strcmp(name, "pci.ids") == 0)
            osinfo_loader_process_file_reg_pci(loader, file, info, &error);
        break;

    case G_FILE_TYPE_DIRECTORY:
        osinfo_loader_process_file_dir(loader, file, info, &error);
        break;

    default:
        break;
    }

    g_object_unref(info);

    if (error)
        g_propagate_error(err, error);
}

/**
 * osinfo_loader_get_db:
 * @loader: the loader object
 *
 * Retrieves the database being populated
 *
 * Returns: (transfer none): the database
 */
OsinfoDb *osinfo_loader_get_db(OsinfoLoader *loader)
{
    g_return_val_if_fail(OSINFO_IS_LOADER(loader), NULL);

    return loader->priv->db;
}

/**
 * osinfo_loader_process_path:
 * @loader: the loader object
 * @path: the fully qualified path
 * @err: (out): filled with error information upon failure
 *
 * Loads data from the specified path. If the path
 * points to a file, that will be loaded as XML
 * Otherwise it can point to a directory which will
 * be recursively traversed, loading all files as XML.
 */
void osinfo_loader_process_path(OsinfoLoader *loader,
                                const gchar *path,
                                GError **err)
{
    GFile *file = g_file_new_for_path(path);
    osinfo_loader_process_file(loader,
                               file,
                               FALSE,
                               err);
    g_object_unref(file);
}

/**
 * osinfo_loader_process_uri:
 * @loader: the loader object
 * @uri: the data source URI
 * @err: (out): filled with error information upon failure
 *
 * Loads data from the specified URI. If the URI
 * points to a file, that will be loaded as XML
 * Otherwise it can point to a directory which will
 * be recursively traversed, loading all files as XML.
 */
void osinfo_loader_process_uri(OsinfoLoader *loader,
                               const gchar *uri,
                               GError **err)
{
    GFile *file = g_file_new_for_uri(uri);
    osinfo_loader_process_file(loader,
                               file,
                               FALSE,
                               err);
    g_object_unref(file);
}


void osinfo_loader_process_default_path(OsinfoLoader *loader, GError **err)
{
    GError *error = NULL;

    osinfo_loader_process_system_path(loader, &error);
    if (error)
        goto error;

    osinfo_loader_process_local_path(loader, &error);
    if (error)
        goto error;

    osinfo_loader_process_user_path(loader, &error);
    if (error)
        goto error;

    return;

 error:
    g_propagate_error(err, error);
    return;
}

/**
 * osinfo_loader_process_system_path:
 * @loader: the loader object
 * @err: (out): filled with error information upon failure
 *
 * Loads data from the default paths.
 */
void osinfo_loader_process_system_path(OsinfoLoader *loader,
                                       GError **err)
{
    GFile *file;
    gchar *dbdir;
    const gchar *path = g_getenv("OSINFO_DATA_DIR");
    if (!path)
        path = PKG_DATA_DIR;

    dbdir = g_strdup_printf("%s/db", path);
    file = g_file_new_for_path(dbdir);
    osinfo_loader_process_file(loader,
                               file,
                               FALSE,
                               err);
    g_object_unref(file);
    g_free(dbdir);
}

void osinfo_loader_process_local_path(OsinfoLoader *loader, GError **err)
{
    GFile *file;
    const gchar *dbdir = SYS_CONF_DIR "/libosinfo/db";

    file = g_file_new_for_path(dbdir);
    osinfo_loader_process_file(loader,
                               file,
                               TRUE,
                               err);
    g_object_unref(file);
}

void osinfo_loader_process_user_path(OsinfoLoader *loader, GError **err)
{
    GFile *file;
    gchar *dbdir;
    const gchar *configdir = g_get_user_config_dir();

    dbdir = g_strdup_printf("%s/libosinfo/db", configdir);
    file = g_file_new_for_path(dbdir);
    osinfo_loader_process_file(loader,
                               file,
                               TRUE,
                               err);
    g_object_unref(file);
    g_free(dbdir);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
