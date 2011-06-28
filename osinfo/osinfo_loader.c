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

#include <gio/gio.h>

#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlreader.h>

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

static void osinfo_loader_finalize (GObject *object);

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

    g_klass->finalize = osinfo_loader_finalize;

    g_type_class_add_private (klass, sizeof (OsinfoLoaderPrivate));
}

static void
osinfo_loader_init (OsinfoLoader *loader)
{
    OsinfoLoaderPrivate *priv;
    loader->priv = priv = OSINFO_LOADER_GET_PRIVATE(loader);

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

static void osinfo_loader_entity(OsinfoLoader *loader,
                                 OsinfoEntity *entity,
                                 const gchar *const *keys,
                                 xmlXPathContextPtr ctxt,
                                 xmlNodePtr root,
                                 GError **err)
{
    int i = 0;

    /* Standard well-known keys first, allow single value only */
    for (i = 0 ; keys[i] != NULL ; i++) {
        gchar *xpath = g_strdup_printf("string(./%s)", keys[i]);
        gchar *value = osinfo_loader_string(xpath, ctxt, err);
        g_free(xpath);
        if (*err)
            return;

        if (value) {
            osinfo_entity_set_param(entity, keys[i], value);
            g_free(value);
        }
    }

    /* Then any site specific custom keys. x-... Can be repeated */
    xmlNodePtr *custom = NULL;
    int ncustom = osinfo_loader_nodeset("./*[substring(name(),1,2)='x-']", ctxt, &custom, err);
    if (*err)
        return;

    for (i = 0 ; i < ncustom ; i++) {
        xmlNodePtr param = custom[i];

        if (!param->children ||
            param->children->type != XML_TEXT_NODE) {
            OSINFO_ERROR(err, "Expected a text node attribute value");
            goto cleanup;
        }

        osinfo_entity_add_param(entity,
                                (const char *)custom[i]->name,
                                (const char *)custom[i]->children->content);
    }

 cleanup:
    g_free(custom);
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

static void osinfo_loader_device(OsinfoLoader *loader,
                                 xmlXPathContextPtr ctxt,
                                 xmlNodePtr root,
                                 GError **err)
{
    gchar *id = (gchar *)xmlGetProp(root, BAD_CAST "id");
    const gchar *const keys[] = {
        OSINFO_DEVICE_PROP_VENDOR,
        OSINFO_DEVICE_PROP_PRODUCT,
        OSINFO_DEVICE_PROP_BUS_TYPE,
        OSINFO_DEVICE_PROP_CLASS,
        OSINFO_DEVICE_PROP_NAME,
        NULL,
    };
    if (!id) {
        OSINFO_ERROR(err, "Missing device id property");
        return;
    }

    OsinfoDevice *device = osinfo_loader_get_device(loader, id);
    g_free(id);

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
    if (*err)
        return;

    for (i = 0 ; i < nrelated ; i++) {
        const gchar *keys[] = {
            OSINFO_DEVICELINK_PROP_DRIVER,
            NULL,
        };
        gchar *id = (gchar *)xmlGetProp(related[i], BAD_CAST "id");
        if (!id) {
            OSINFO_ERROR(err, "Missing device link id property");
            goto cleanup;
        }
        OsinfoDevice *dev = osinfo_loader_get_device(loader, id);
        g_free(id);

        OsinfoDeviceLink *link = NULL;
        if (OSINFO_IS_PLATFORM(entity)) {
            link = osinfo_platform_add_device(OSINFO_PLATFORM(entity), dev);
        } else if (OSINFO_IS_OS(entity)) {
            link = osinfo_os_add_device(OSINFO_OS(entity), dev);
        } else if (OSINFO_IS_DEPLOYMENT(entity)) {
            link = osinfo_deployment_add_device(OSINFO_DEPLOYMENT(entity), dev);
        }

        xmlNodePtr saved = ctxt->node;
        ctxt->node = related[i];
        osinfo_loader_entity(loader, OSINFO_ENTITY(link), keys, ctxt, root, err);
        ctxt->node = saved;
        if (*err)
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
    if (*err)
        return;

    for (i = 0 ; i < nrelated ; i++) {
        gchar *id = (gchar *)xmlGetProp(related[i], BAD_CAST "id");
        if (!id) {
            OSINFO_ERROR(err, "Missing product upgrades id property");
            goto cleanup;
        }
        OsinfoProduct *relproduct;
        if (OSINFO_IS_PLATFORM(product))
            relproduct = OSINFO_PRODUCT(osinfo_loader_get_platform(loader, id));
        else
            relproduct = OSINFO_PRODUCT(osinfo_loader_get_os(loader, id));
        g_free(id);

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
    const gchar *const keys[] = {
        OSINFO_PRODUCT_PROP_NAME,
        OSINFO_PRODUCT_PROP_VENDOR,
        OSINFO_PRODUCT_PROP_VERSION,
        OSINFO_PRODUCT_PROP_SHORT_ID,
        NULL,
    };

    osinfo_loader_entity(loader, OSINFO_ENTITY(product), keys, ctxt, root, err);
    if (*err)
        return;


    osinfo_loader_product_relshp(loader, product,
                                 OSINFO_PRODUCT_RELATIONSHIP_DERIVES_FROM,
                                 "./derives-from",
                                 ctxt,
                                 root,
                                 err);
    if (*err)
        return;

    osinfo_loader_product_relshp(loader, product,
                                 OSINFO_PRODUCT_RELATIONSHIP_CLONES,
                                 "./clones",
                                 ctxt,
                                 root,
                                 err);
    if (*err)
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
    const gchar *const keys[] = {
        NULL,
    };
    if (!id) {
        OSINFO_ERROR(err, "Missing platform id property");
        return;
    }

    OsinfoPlatform *platform = osinfo_loader_get_platform(loader, id);
    g_free(id);

    osinfo_loader_entity(loader, OSINFO_ENTITY(platform), keys, ctxt, root, err);
    if (*err)
        return;

    osinfo_loader_product(loader, OSINFO_PRODUCT(platform), ctxt, root, err);
    if (*err)
        return;

    osinfo_loader_device_link(loader, OSINFO_ENTITY(platform),
                              "./devices/device", ctxt, root, err);
    if (*err)
        return;
}

static void osinfo_loader_deployment(OsinfoLoader *loader,
                                     xmlXPathContextPtr ctxt,
                                     xmlNodePtr root,
                                     GError **err)
{
    gchar *id = (gchar *)xmlGetProp(root, BAD_CAST "id");
    const gchar *const keys[] = {
        NULL
    };
    if (!id) {
        OSINFO_ERROR(err, "Missing deployment id property");
        return;
    }

    gchar *osid = osinfo_loader_string("string(./os/@id)", ctxt, err);
    if (!osid && 0) {
        OSINFO_ERROR(err, "Missing deployment os id property");
        g_free(id);
        return;
    }
    OsinfoOs *os = osinfo_loader_get_os(loader, osid);
    g_free(osid);

    gchar *platformid = osinfo_loader_string("string(./platform/@id)", ctxt, err);
    if (!platformid) {
        OSINFO_ERROR(err, "Missing deployment platform id property");
        g_free(id);
        return;
    }
    OsinfoPlatform *platform = osinfo_loader_get_platform(loader, platformid);
    g_free(platformid);

    OsinfoDeployment *deployment = osinfo_deployment_new(id, os, platform);
    g_free(id);

    osinfo_loader_entity(loader, OSINFO_ENTITY(deployment), keys, ctxt, root, err);
    if (*err)
        return;

    osinfo_loader_device_link(loader, OSINFO_ENTITY(deployment),
                              "./devices/device", ctxt, root, err);
    if (*err)
        return;

    osinfo_db_add_deployment(loader->priv->db, deployment);
}

static void osinfo_loader_os(OsinfoLoader *loader,
                             xmlXPathContextPtr ctxt,
                             xmlNodePtr root,
                             GError **err)
{
    gchar *id = (gchar *)xmlGetProp(root, BAD_CAST "id");
    const gchar *const keys[] = {
        NULL
    };
    if (!id) {
        OSINFO_ERROR(err, "Missing os id property");
        return;
    }

    OsinfoOs *os = osinfo_loader_get_os(loader, id);
    g_free(id);

    osinfo_loader_entity(loader, OSINFO_ENTITY(os), keys, ctxt, root, err);
    if (*err)
        return;

    osinfo_loader_product(loader, OSINFO_PRODUCT(os), ctxt, root, err);
    if (*err)
        return;

    osinfo_loader_device_link(loader, OSINFO_ENTITY(os),
                              "./devices/device", ctxt, root, err);
    if (*err)
        return;
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
     *   If element tag, and element is not os, platform or device, error
     *   Else, switch on tag type and handle reading in data
     * After loop, return success if no error
     * If there was an error, clean up lib data acquired so far
     */
    xmlNodePtr *oss = NULL;
    xmlNodePtr *devices = NULL;
    xmlNodePtr *platforms = NULL;
    xmlNodePtr *deployments = NULL;

    if (!xmlStrEqual(root->name, BAD_CAST "libosinfo")) {
        OSINFO_ERROR(err, "Incorrect root element");
        return;
    }

    int ndevice = osinfo_loader_nodeset("./device", ctxt, &devices, err);
    if (*err)
        goto cleanup;

    int i;
    for (i = 0 ; i < ndevice ; i++) {
        xmlNodePtr saved = ctxt->node;
        ctxt->node = devices[i];
        osinfo_loader_device(loader, ctxt, devices[i], err);
        ctxt->node = saved;
        if (*err)
            goto cleanup;
    }

    int nplatform = osinfo_loader_nodeset("./platform", ctxt, &platforms, err);
    if (*err)
        goto cleanup;

    for (i = 0 ; i < nplatform ; i++) {
        xmlNodePtr saved = ctxt->node;
        ctxt->node = platforms[i];
        osinfo_loader_platform(loader, ctxt, platforms[i], err);
        ctxt->node = saved;
        if (*err)
            goto cleanup;
    }

    int nos = osinfo_loader_nodeset("./os", ctxt, &oss, err);
    if (*err)
        goto cleanup;

    for (i = 0 ; i < nos ; i++) {
        xmlNodePtr saved = ctxt->node;
        ctxt->node = oss[i];
        osinfo_loader_os(loader, ctxt, oss[i], err);
        ctxt->node = saved;
        if (*err)
            goto cleanup;
    }

    int ndeployment = osinfo_loader_nodeset("./deployment", ctxt, &deployments, err);
    if (*err)
        goto cleanup;

    for (i = 0 ; i < ndeployment ; i++) {
        xmlNodePtr saved = ctxt->node;
        ctxt->node = deployments[i];
        osinfo_loader_deployment(loader, ctxt, deployments[i], err);
        ctxt->node = saved;
        if (*err)
            goto cleanup;
    }


 cleanup:
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
        if (*err == NULL) {
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
        OSINFO_ERROR(err, "Unable to construct parser context");
        goto cleanup;
    }

    pctxt->_private = err;
    pctxt->sax->error = catchXMLError;

    xml = xmlCtxtReadDoc(pctxt, BAD_CAST xmlStr, src, NULL,
                         XML_PARSE_NOENT | XML_PARSE_NONET |
                         XML_PARSE_NOWARNING);
    if (!xml)
        goto cleanup;

    root = xmlDocGetRootElement(xml);

    if (!root) {
        OSINFO_ERROR(err, "Missing root element");
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
    if (*err)
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

                /* Pretend we 'use' these variables to get around
                 * 'gcc' warnings about set-but-not-read vars */
                if (subvendor_id || subdevice_id || subsystem)
                    subsystem = subsystem;
            } else {
                FREE_BUF(device_buf);
                WANT_ID(device_id);
                WANT_REST(device);
                SAVE_BUF(device_buf);

                /* Trick gcc set-by-not-read-vars warning */
                if (device)
                    device = device;

                gchar *id = g_strdup_printf("%s/%s/%s",
                                            baseURI, vendor_id, device_id);

                OsinfoDevice *dev = osinfo_loader_get_device(loader, id);
                osinfo_entity_set_param(OSINFO_ENTITY(dev),
                                        OSINFO_DEVICE_PROP_VENDOR,
                                        vendor_id);
                osinfo_entity_set_param(OSINFO_ENTITY(dev),
                                        OSINFO_DEVICE_PROP_PRODUCT,
                                        device_id);
                g_free(id);
            }
        } else {
            FREE_BUF(vendor_buf);
            WANT_ID(vendor_id);
            WANT_REST(vendor);
            SAVE_BUF(vendor_buf);

            /* Trick gcc set-by-not-read-vars warning */
            if (vendor)
                vendor = vendor;
        }

    done:
        g_free(data);
        if (*err)
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
    if (*err)
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
    if (*err)
        return;

    GFileInfo *child;
    while ((child = g_file_enumerator_next_file(ents, NULL, err)) != NULL) {
        const gchar *name = g_file_info_get_name(child);
        GFile *ent = g_file_get_child(file, name);

        osinfo_loader_process_file(loader, ent, err);

        g_object_unref(ent);
        g_object_unref(child);

        if (*err)
            break;
    }

    g_object_unref(ents);
}

static void
osinfo_loader_process_file(OsinfoLoader *loader,
                           GFile *file,
                           GError **err)
{
    size_t len;
    GFileInfo *info = g_file_query_info(file,
                                        "standard::*",
                                        G_FILE_QUERY_INFO_NONE,
                                        NULL,
                                        err);
    const char *name = g_file_info_get_name(info);

    if (*err)
        return;

    GFileType type = g_file_info_get_attribute_uint32(info,
                                                      G_FILE_ATTRIBUTE_STANDARD_TYPE);

    switch (type) {
    case G_FILE_TYPE_REGULAR:
        len = strlen(name);
        if (strcmp(name + len - 4, ".xml") == 0)
            osinfo_loader_process_file_reg_xml(loader, file, info, err);
        else if (strcmp(name, "usb.ids") == 0)
            osinfo_loader_process_file_reg_usb(loader, file, info, err);
        else if (strcmp(name, "pci.ids") == 0)
            osinfo_loader_process_file_reg_pci(loader, file, info, err);
        break;

    case G_FILE_TYPE_DIRECTORY:
        osinfo_loader_process_file_dir(loader, file, info, err);
        break;

    default:
        break;
    }

    g_object_unref(info);
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
                               err);
    g_object_unref(file);
}

/**
 * osinfo_loader_process_default_path:
 * @loader: the loader object
 * @err: (out): filled with error information upon failure
 *
 * Loads data from the default path.
 */
void osinfo_loader_process_default_path(OsinfoLoader *loader,
                                        GError **err)
{
    GFile *file = g_file_new_for_path(DATA_DIR);
    osinfo_loader_process_file(loader,
                               file,
                               err);
    g_object_unref(file);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
