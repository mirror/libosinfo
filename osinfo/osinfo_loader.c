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

#include <dirent.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlreader.h>

G_DEFINE_TYPE (OsinfoLoader, osinfo_loader, G_TYPE_OBJECT);

#define OSINFO_LOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_LOADER, OsinfoLoaderPrivate))

struct _OsinfoLoaderPrivate
{
    OsinfoDb *db;
};

static void osinfo_loader_finalize (GObject *object);

static void
osinfo_loader_finalize (GObject *object)
{
    OsinfoLoader *self = OSINFO_LOADER (object);

    g_object_unref(self->priv->db);

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
osinfo_loader_init (OsinfoLoader *self)
{
    OsinfoLoaderPrivate *priv;
    self->priv = priv = OSINFO_LOADER_GET_PRIVATE(self);

    self->priv->db = osinfo_db_new();
}

/** PUBLIC METHODS */

OsinfoLoader *osinfo_loader_new(void)
{
    return g_object_new(OSINFO_TYPE_LOADER, NULL);
}


#define OSINFO_ERROR(err, msg) \
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
    for (i = 0 ; keys[i] != NULL ; i++) {
        gchar *xpath = g_strdup_printf("string(./%s)", keys[i]);
	gchar *value = osinfo_loader_string(xpath, ctxt, err);
	g_free(xpath);
	if (*err)
	    return;

	if (value) {
	    osinfo_entity_add_param(entity, keys[i], value);
	    g_free(value);
	}
    }
}


static OsinfoDevice *osinfo_loader_get_device(OsinfoLoader *loader,
					      const gchar *id)
{
    OsinfoDevice *dev = osinfo_db_get_device(loader->priv->db, id);
    if (!dev) {
        OsinfoDeviceList *list = osinfo_db_get_device_list(loader->priv->db);
        dev = osinfo_device_new(id);
	osinfo_list_add(OSINFO_LIST(list), OSINFO_ENTITY(dev));
	g_object_unref(dev);
    }
    return dev;
}


static OsinfoOs *osinfo_loader_get_os(OsinfoLoader *loader,
				      const gchar *id)
{
    OsinfoOs *os = osinfo_db_get_os(loader->priv->db, id);
    if (!os) {
        OsinfoOsList *list = osinfo_db_get_os_list(loader->priv->db);
        os = osinfo_os_new(id);
	osinfo_list_add(OSINFO_LIST(list), OSINFO_ENTITY(os));
	g_object_unref(os);
    }
    return os;
}


static OsinfoHypervisor *osinfo_loader_get_hypervisor(OsinfoLoader *loader,
						      const gchar *id)
{
    OsinfoHypervisor *hv = osinfo_db_get_hypervisor(loader->priv->db, id);
    if (!hv) {
        OsinfoHypervisorList *list = osinfo_db_get_hypervisor_list(loader->priv->db);
        hv = osinfo_hypervisor_new(id);
	osinfo_list_add(OSINFO_LIST(list), OSINFO_ENTITY(hv));
	g_object_unref(hv);
    }
    return hv;
}


static void osinfo_loader_device(OsinfoLoader *loader,
				 xmlXPathContextPtr ctxt,
				 xmlNodePtr root,
				 GError **err)
{
    gchar *id = (gchar *)xmlGetProp(root, BAD_CAST "id");
    const gchar *const keys[] = {
      "vendor", "product", "bus-type", "class", "name", NULL,
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
				      OsinfoOs *os,
				      OsinfoHypervisor *hv,
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
	    OSINFO_ERROR(err, "Missing device link id property");
	    goto cleanup;
	}
	OsinfoDevice *dev = osinfo_loader_get_device(loader, id);
	g_free(id);

	if (os) {
	    osinfo_os_add_device(os, hv, dev, "foo");
	} else if (hv) {
	    osinfo_hypervisor_add_device(hv, dev, "foo");
	}
    }

 cleanup:
    g_free(related);
}


static void osinfo_loader_hypervisor(OsinfoLoader *loader,
				     xmlXPathContextPtr ctxt,
				     xmlNodePtr root,
				     GError **err)
{
    gchar *id = (gchar *)xmlGetProp(root, BAD_CAST "id");
    const gchar *const keys[] = {
      "name", "version", NULL,
    };
    if (!id) {
        OSINFO_ERROR(err, "Missing hypervisor id property");
	return;
    }

    OsinfoHypervisor *hypervisor = osinfo_loader_get_hypervisor(loader, id);
    g_free(id);

    osinfo_loader_entity(loader, OSINFO_ENTITY(hypervisor), keys, ctxt, root, err);
    if (*err)
        return;

    osinfo_loader_device_link(loader, NULL, hypervisor,
			      "./devices/device", ctxt, root, err);
    if (*err)
        return;
}


static void osinfo_loader_os_relshp(OsinfoLoader *loader,
				    OsinfoOs *os,
				    OsinfoOsRelationship relshp,
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
	    OSINFO_ERROR(err, "Missing os upgrades id property");
	    goto cleanup;
	}
	OsinfoOs *relos = osinfo_loader_get_os(loader, id);
	g_free(id);

	osinfo_os_add_related_os(os, relshp, relos);
    }

 cleanup:
    g_free(related);
}


static void osinfo_loader_os_hypervisor(OsinfoLoader *loader,
					OsinfoOs *os,
					xmlXPathContextPtr ctxt,
					xmlNodePtr root,
					GError **err)
{
    xmlNodePtr *hvs = NULL;
    int nhvs = osinfo_loader_nodeset("./hypervisor", ctxt, &hvs, err);
    int i;
    if (*err)
        return;

    for (i = 0 ; i < nhvs ; i++) {
	gchar *id = (gchar *)xmlGetProp(hvs[i], BAD_CAST "id");
	if (!id) {
	    OSINFO_ERROR(err, "Missing os hypervisor id property");
	    goto cleanup;
	}
	OsinfoHypervisor *hv = osinfo_loader_get_hypervisor(loader, id);
	g_free(id);

	xmlNodePtr saved = ctxt->node;
	ctxt->node = hvs[i];
	osinfo_loader_device_link(loader, os, hv,
				  "./devices/device", ctxt, hvs[i], err);
	ctxt->node = saved;
	if (*err)
	    goto cleanup;
    }

 cleanup:
    g_free(hvs);
}


static void osinfo_loader_os(OsinfoLoader *loader,
			     xmlXPathContextPtr ctxt,
			     xmlNodePtr root,
			     GError **err)
{
    gchar *id = (gchar *)xmlGetProp(root, BAD_CAST "id");
    const gchar *const keys[] = {
      "name", "version", "short-id", NULL
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

    osinfo_loader_os_relshp(loader, os,
			    OSINFO_OS_RELATIONSHIP_DERIVES_FROM,
			    "./derives-from",
			    ctxt,
			    root,
			    err);
    if (*err)
        return;

    osinfo_loader_os_relshp(loader, os,
			    OSINFO_OS_RELATIONSHIP_CLONES,
			    "./clones",
			    ctxt,
			    root,
			    err);
    if (*err)
        return;

    osinfo_loader_os_relshp(loader, os,
			    OSINFO_OS_RELATIONSHIP_UPGRADES,
			    "./upgrades",
			    ctxt,
			    root,
			    err);
    if (*err)
        return;

    osinfo_loader_os_hypervisor(loader, os, ctxt, root, err);
    if (*err)
        return;

    osinfo_loader_device_link(loader, os, NULL,
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
     *   If element tag, and element is not os, hypervisor or device, error
     *   Else, switch on tag type and handle reading in data
     * After loop, return success if no error
     * If there was an error, clean up lib data acquired so far
     */
    xmlNodePtr *oss = NULL;
    xmlNodePtr *devices = NULL;
    xmlNodePtr *hypervisors = NULL;

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

    int nhypervisor = osinfo_loader_nodeset("./hypervisor", ctxt, &hypervisors, err);
    if (*err)
        goto cleanup;

    for (i = 0 ; i < nhypervisor ; i++) {
        xmlNodePtr saved = ctxt->node;
	ctxt->node = hypervisors[i];
	osinfo_loader_hypervisor(loader, ctxt, hypervisors[i], err);
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

 cleanup:
    g_free(hypervisors);
    g_free(oss);
    g_free(devices);
}


static void
catchXMLError(void *ctx, const char *msg ATTRIBUTE_UNUSED, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;

    if (ctxt && ctxt->_private) {
        gchar *xmlmsg = g_strdup_printf("at line %d: %s",
					ctxt->lastError.line,
					ctxt->lastError.message);
	OSINFO_ERROR(ctxt->_private, xmlmsg);
	g_free(xmlmsg);
    }
}

static void osinfo_loader_file(OsinfoLoader *loader,
				 const char *dir,
				 const char *filename,
				 GError **err)
{
    xmlParserCtxtPtr pctxt;
    xmlXPathContextPtr ctxt = NULL;
    xmlDocPtr xml = NULL;
    xmlNodePtr root;
    char *rel_name = g_strdup_printf("%s/%s", dir, filename);

    /* Set up a parser context so we can catch the details of XML errors. */
    pctxt = xmlNewParserCtxt();
    if (!pctxt || !pctxt->sax) {
        OSINFO_ERROR(err, "Unable to construct parser context");
        goto cleanup;
    }

    pctxt->_private = err;
    pctxt->sax->error = catchXMLError;

    xml = xmlCtxtReadFile(pctxt, rel_name, NULL,
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
    g_free(rel_name);
}

void osinfo_loader_process(OsinfoLoader *loader,
			   const gchar *dir,
			   GError **err)
{
    DIR* d;
    struct dirent *dp;

    /* Get directory with backing data. Defaults to CWD */

    /* Get XML files in directory */
    d = opendir(dir);
    if (!d) {
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), 0,
			    "unable to read backing dir");
        goto cleanup;
    }

    while ((dp=readdir(d)) != NULL) {
        if (dp->d_name[0] == '.')
	    continue;
        osinfo_loader_file(loader, dir, dp->d_name, err);
        if (*err != NULL)
            break;
    }
    closedir(d);

cleanup:
    xmlCleanupParser();
}
