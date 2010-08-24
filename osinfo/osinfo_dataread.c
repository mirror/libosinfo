#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <libxml/xmlreader.h>

#include <osinfo/osinfo.h>

#ifdef LIBXML_READER_ENABLED

#define TEXT_NODE 3
#define ELEMENT_NODE 1
#define END_NODE 15
#define WHITESPACE_NODE 14
#define COMMENT_NODE 8

void osinfo_dataread(OsinfoDb *db, GError **err);

/*
 * TODO:
 * 1. More robust handling of files that are in bad format
 * 2. Error messages for parsing
 */

/*
 * Notes regarding parsing XML Data:
 *
 * The top level tag is <libosinfo>. The next highest level consists of
 * <device>, <hypervisor> and <os>. Each tag may be empty (of the form <tag />)
 * or containing data (of the form <tag>...</tag>). After parsing an object, the
 * cursor will be located at the closing tag for the object (which, for an empty
 * object, is the same as the starting tag for the object).
 */

struct __osinfoDbRet {
    OsinfoDb *db;
    GError **err;
};

#define OSINFO_ERROR(err, msg) \
  g_set_error_literal((err), g_quark_from_static_string("libosinfo"), 0, (msg));

static gboolean __osinfoResolveDeviceLink(gpointer key, gpointer value, gpointer data)
{
    gchar *id = (gchar *) key;
    struct __osinfoDeviceLink *devLink = (struct __osinfoDeviceLink *) value;
    struct __osinfoDbRet *dbRet = (struct __osinfoDbRet *) data;
    OsinfoDb *db = dbRet->db;
    OsinfoDeviceList *devices = osinfo_db_get_device_list(db);

    OsinfoDevice *dev = OSINFO_DEVICE(osinfo_list_find_by_id(OSINFO_LIST(devices), id));
    if (!dev) {
        OSINFO_ERROR(dbRet->err, "missing device");
	return TRUE;
    }

    devLink->dev = dev;
    return FALSE;
}

static gboolean __osinfoResolveSectionDevices(gpointer key, gpointer value, gpointer data)
{
    g_return_val_if_fail(value != NULL, TRUE);

    struct __osinfoDbRet *dbRet = (struct __osinfoDbRet *) data;
    GTree *section = value;

    g_tree_foreach(section, __osinfoResolveDeviceLink, dbRet);
    if (*dbRet->err)
        return TRUE;
    return FALSE;
}

static void __osinfoResolveHvLink(gpointer key, gpointer value, gpointer data)
{
    gchar *hvId = (gchar *) key;
    struct __osinfoDbRet *dbRet = (struct __osinfoDbRet *) data;
    OsinfoDb *db = dbRet->db;
    struct __osinfoHvSection *hvSection = (struct __osinfoHvSection *) value;
    OsinfoHypervisor *hv;
    OsinfoHypervisorList *hypervisors = osinfo_db_get_hypervisor_list(db);

    g_tree_foreach(hvSection->sections, __osinfoResolveSectionDevices, dbRet);
    if (*dbRet->err)
        return;

    hv = OSINFO_HYPERVISOR(osinfo_list_find_by_id(OSINFO_LIST(hypervisors), hvId));
    if (!hv) {
        OSINFO_ERROR(dbRet->err, "missing hypervisor");
        return;
    }

    hvSection->hv = hv;
}

static gboolean __osinfoResolveOsLink(gpointer key, gpointer value, gpointer data)
{
    gchar *targetOsId = (gchar *) key;
    struct __osinfoDbRet *dbRet = (struct __osinfoDbRet *) data;
    OsinfoDb *db = dbRet->db;
    struct __osinfoOsLink *osLink = (struct __osinfoOsLink *) value;
    OsinfoOs *targetOs;
    OsinfoOsList *oslist = osinfo_db_get_os_list(db);

    targetOs = OSINFO_OS(osinfo_list_find_by_id(OSINFO_LIST(oslist), targetOsId));
    if (!targetOs) {
        OSINFO_ERROR(dbRet->err, "missing os");
        return TRUE;
    }

    osLink->directObjectOs = targetOs;
    return FALSE;
}

static gboolean __osinfoFixOsLinks(OsinfoList *list, OsinfoEntity *entity, gpointer data)
{
    g_return_val_if_fail(OSINFO_OS(entity), TRUE);

    struct __osinfoDbRet *dbRet = data;
    OsinfoOs *os = OSINFO_OS(entity);

    g_tree_foreach(os->priv->sections, __osinfoResolveSectionDevices, dbRet);
    if (*dbRet->err)
        return TRUE;

    g_tree_foreach(os->priv->relationshipsByOs, __osinfoResolveOsLink, dbRet);
    if (*dbRet->err)
        return TRUE;

    g_hash_table_foreach(os->priv->hypervisors, __osinfoResolveHvLink, dbRet);
    if (*dbRet->err)
        return TRUE;

    return FALSE;
}

static void __osinfoFixObjLinks(OsinfoDb *db, GError **err)
{
    g_return_if_fail(OSINFO_IS_DB(db));

    struct __osinfoDbRet dbRet = {db, err };
    OsinfoOsList *oses = osinfo_db_get_os_list(db);

    osinfo_list_foreach(OSINFO_LIST(oses), __osinfoFixOsLinks, &dbRet);
}

static int __osinfoProcessTag(xmlTextReaderPtr reader, char** ptr_to_key, char** ptr_to_val)
{
    int node_type, ret, err = 0;
    char* key = NULL;
    char* val = NULL;
    const gchar* node_name, * end_node_name, * xml_value;

    node_name = (const gchar *)xmlTextReaderConstName(reader);
    if (!node_name)
        goto error;

    key = g_strdup((const gchar *)node_name);

    /* Advance to next node */
    ret = xmlTextReaderRead(reader);
    if (ret != 1)
        goto error;

    /* Ensure node is a text node */
    node_type = xmlTextReaderNodeType(reader);
    if (node_type != TEXT_NODE)
        goto error;

    /* Store the value of the text node */
    xml_value = (const gchar *)xmlTextReaderConstValue(reader);
    if (!xml_value)
        goto error;

    val = g_strdup((const gchar *)xml_value);

    /* Advance to the next node */
    ret = xmlTextReaderRead(reader);
    if (ret != 1)
        goto error;

    /* Ensure the node is an end node for the tracked start node */
    node_type = xmlTextReaderNodeType(reader);
    end_node_name = (const gchar *)xmlTextReaderConstName(reader);
    if (node_type != END_NODE ||
        !end_node_name ||
        strcmp((const gchar *)end_node_name,
	       (const gchar *)node_name) != 0)
            goto error;

    /* Now we have key and val with no errors so we return with success */
    *ptr_to_key = key;
    *ptr_to_val = val;
    return 0;

error:
    free(key);
    free(val);
    *ptr_to_key = NULL;
    *ptr_to_val = NULL;
    if (err == 0)
        err = -EINVAL;
    return err;
}

static int __osinfoProcessDevSection(xmlTextReaderPtr reader,
                                     GTree *section, GTree *sectionAsList)
{
    int err, empty, node_type;
    gchar * sectionType, * id, * key = NULL, * driver = NULL;
    const gchar * name;

    if (!section)
        return -EINVAL;

    sectionType = (gchar *)xmlTextReaderGetAttribute(reader, BAD_CAST "type");
    empty = xmlTextReaderIsEmptyElement(reader);

    if (!sectionType)
        return -EINVAL;

    /* If no devices in section then we are done */
    if (empty)
        return 0;

    /* Otherwise, read in devices and add to section */
    for (;;) {
        /* Advance to next node */
        err = xmlTextReaderRead(reader);
        if (err != 1) {
            err = -EINVAL;
            goto error;
        }

        node_type = xmlTextReaderNodeType(reader);
        name = (const gchar *)xmlTextReaderConstName(reader);

        /* If end of section, break */
        if (node_type == END_NODE && strcmp(name, "section") == 0)
            break;

        /* If node is not start of an element, continue */
        if (node_type != ELEMENT_NODE)
            continue;

        /* Element within section needs to be of type device */
        if (strcmp(name, "device") != 0) {
            err = -EINVAL;
            goto error;
        }

        id = (gchar *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
        empty = xmlTextReaderIsEmptyElement(reader);

        if (!id) {
            err = -EINVAL;
            goto error;
        }

        if (!empty) {
            err = __osinfoProcessTag(reader, &key, &driver);
            if (err != 0 || !key || !driver)
                goto error;
            free(key);
            key = NULL; /* In case the next malloc fails, avoid a double free */
        }

        // Alright, we have the id and driver
        err = __osinfoAddDeviceToSection(section, sectionAsList, sectionType, id, driver);
        free (driver);
        driver = NULL;
        free (id);
        id = NULL;
        if (err != 0)
            goto error;
    }
    free(sectionType);

    return 0;

error:
    free(sectionType);
    free(key);
    free(driver);
    return err;
}

static int __osinfoProcessOsHvLink(xmlTextReaderPtr reader,
                                   OsinfoOs *os)
{
    /*
     * Get id for hypervisor else fail
     * While true:
     *   Advance to next node
     *   If node is end of hypervisor break
     *   If node is not element type continue
     *   If node is element type and not section fail
     *   Else handle section and add to hv_link
     * If fail delete hv_link so far
     * On success add hv_link to os
     */
    int empty, node_type, err;
    char* id;
    const gchar* name;
    struct __osinfoHvSection *hvSection;

    id = (gchar *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
    empty = xmlTextReaderIsEmptyElement(reader);

    if (!id)
        return -EINVAL;

    hvSection = __osinfoAddHypervisorSectionToOs(os, id);
    free(id);
    if (!hvSection)
        return -EINVAL;

    if (empty)
        goto finished;

    for (;;) {
        /* Advance to next node */
        err = xmlTextReaderRead(reader);
        if (err != 1) {
            err = -EINVAL;
            goto error;
        }

        node_type = xmlTextReaderNodeType(reader);
        name = (const gchar *)xmlTextReaderConstName(reader);
        if (node_type == -1 || !name) {
            err = -EINVAL;
            goto error;
        }

        /* If end of hv link, break */
        if (node_type == END_NODE && strcmp(name, "hypervisor") == 0)
            break;

        /* If node is not start of an element, continue */
        if (node_type != ELEMENT_NODE)
            continue;

        /* Ensure it is element node of type 'section' else fail */
        if (strcmp(name, "section") != 0) {
            err = -EINVAL;
            goto error;
        }

        /* Process device type info for this <os, hv> combination */
        err = __osinfoProcessDevSection(reader, hvSection->sections, hvSection->sectionsAsList);
        if (err != 0)
            goto error;
    }

finished:
    return 0;

error:
    return err;
}

static int __osinfoProcessOsRelationship(xmlTextReaderPtr reader,
                                         OsinfoOs *os,
                                         osinfoRelationship relationship)
{
    int empty, ret;
    gchar *id;

    id = (gchar *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
    empty = xmlTextReaderIsEmptyElement(reader);
    if (!empty || !id) {
        free(id);
        return -EINVAL;
    }

    ret = __osinfoAddOsRelationship (os, id, relationship);
    free(id);
    return ret;
}

static int __osinfoProcessOs(OsinfoDb *db,
                          xmlTextReaderPtr reader)
{
    /* Cursor is at start of (possibly empty) os node */
    /*
     * First, determine if hv has ID or not, and if tag is empty or not.
     * The following cases can occur:
     * 1. No ID: Return invalid. Parse fails overall.
     * 2. Empty, ID: Make hv with given ID and no data and return.
     * 3. Non-Empty, ID: Make hv, parse data till closing tag, return.
     */

    int empty, node_type, err, ret;
    gchar* id, * key = NULL, * val = NULL;
    const gchar* name;
    OsinfoOs *os;
    OsinfoOsList *oses = osinfo_db_get_os_list(db);

    id = (gchar *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
    empty = xmlTextReaderIsEmptyElement(reader);

    if (!id)
        return -EINVAL;

    os = osinfo_os_new(id);
    free(id);

    if (empty)
        goto finished;

    /* Current node is start of non empty os
     * While true:
     *   Advance to next node
     *   If node == end of os break
     *   If node is not element, continue
     *   If node is element and not section or hypervisor:
     *     Note element name
     *     Advance to next node, ensure type is text else error
     *     Store value, advance to next node
     *     Ensure node is end of current name
     *     Store <key, val> in params list for object
     *   If node is start of section handle section and track device driver
     *   If node is hypervisor handle hypervisor link
     */

    for (;;) {
        /* Advance to next node */
        ret = xmlTextReaderRead(reader);
        if (ret != 1) {
            err = -EINVAL;
            goto cleanup_error;
        }

        node_type = xmlTextReaderNodeType(reader);
        name = (const gchar *)xmlTextReaderConstName(reader);
        if (node_type == -1 || !name) {
            err = -EINVAL;
            goto cleanup_error;
        }
        /* If end of os, break */
        if (node_type == END_NODE && strcmp(name, "os") == 0)
            break;

        /* If node is not start of an element, continue */
        if (node_type != ELEMENT_NODE)
            continue;

        if (strcmp(name, "section") == 0) {
            /* Node is start of device section for os */
            err = __osinfoProcessDevSection(reader, (OSINFO_OS(os))->priv->sections, (OSINFO_OS(os))->priv->sectionsAsList);
            if (err != 0)
                goto cleanup_error;
        }
        else if (strcmp(name, "hypervisor") == 0) {
            err = __osinfoProcessOsHvLink(reader, os);
            if (err != 0)
                goto cleanup_error;
        }
        else if (strcmp(name, "upgrades") == 0) {
            err = __osinfoProcessOsRelationship(reader, os, UPGRADES);
            if (err != 0)
                goto cleanup_error;
        }
        else if (strcmp(name, "clones") == 0) {
            err = __osinfoProcessOsRelationship(reader, os, CLONES);
            if (err != 0)
                goto cleanup_error;
        }
        else if (strcmp(name, "derives-from") == 0) {
            err = __osinfoProcessOsRelationship(reader, os, DERIVES_FROM);
            if (err != 0)
                goto cleanup_error;
        }
        else {
            /* Node is start of element of known name */
            err = __osinfoProcessTag(reader, &key, &val);
            if (err != 0 || !key || !val)
                goto cleanup_error;

            err = osinfo_entity_add_param(OSINFO_ENTITY(os), key, val);
            if (err != 0)
                goto cleanup_error;

            free(key);
            free(val);
        }
    }

finished:
    osinfo_list_add(OSINFO_LIST(oses), OSINFO_ENTITY(os));
    g_object_unref(os);
    return 0;
    /* At end, cursor is at end of os node */

cleanup_error:
    g_object_unref(os);
    return err;
}

static int __osinfoProcessHypervisor(OsinfoDb *db,
                                  xmlTextReaderPtr reader)
{
    /* Cursor is at start of (possibly empty) hypervisor node */

    /*
     * First, determine if hv has ID or not, and if tag is empty or not.
     * The following cases can occur:
     * 1. No ID: Return invalid. Parse fails overall.
     * 2. Empty, ID: Make hv with given ID and no data and return.
     * 3. Non-Empty, ID: Make hv, parse data till closing tag, return.
     */

    int empty, node_type, err, ret;
    gchar *id, *key, *driver;
    const gchar *name;
    OsinfoHypervisor *hv;
    OsinfoHypervisorList *hypervisors = osinfo_db_get_hypervisor_list(db);

    id = (gchar *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
    empty = xmlTextReaderIsEmptyElement(reader);

    if (!id)
        return -EINVAL;

    hv = osinfo_hypervisor_new(id);
    free(id);

    if (empty)
        goto finished;

    /* Current node is start of non empty hv
     * While true:
     *   Advance to next node
     *   If node == end of hv break
     *   If node is not element, continue
     *   If node is element and not section:
     *     Note element name
     *     Advance to next node, ensure type is text else error
     *     Store value, advance to next node
     *     Ensure node is end of current name
     *     Store <key, val> in params list for object
     *   If node is start of section:
     *     Note section type
     *     While true:
     *       Advance to next node
     *       If node is not element continue
     *       If end of section, break
     *       If not empty device node, parse error
     *       If id not defined, parse error
     *       Store id
     *     Store all ids for given section in the HV
     */

    for (;;) {
        /* Advance to next node */
        ret = xmlTextReaderRead(reader);
        if (ret != 1) {
            err = -EINVAL;
            goto cleanup_error;
        }

        node_type = xmlTextReaderNodeType(reader);
        name = (const gchar *)xmlTextReaderConstName(reader);
        if (node_type == -1 || !name) {
            err = -EINVAL;
            goto cleanup_error;
        }
        /* If end of hv, break */
        if (node_type == END_NODE && strcmp(name, "hypervisor") == 0)
            break;

        /* If node is not start of an element, continue */
        if (node_type != ELEMENT_NODE)
            continue;

        /* Element within section needs to be of type device */
        if (strcmp(name, "device") == 0) {
	    id = (gchar *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
	    empty = xmlTextReaderIsEmptyElement(reader);

	    if (!id) {
	      fprintf(stderr, "no id\n");
	        err = -EINVAL;
		goto cleanup_error;
	    }

	    if (!empty) {
	        err = __osinfoProcessTag(reader, &key, &driver);
		if (err != 0 || !key || !driver)
		    goto cleanup_error;
		free(key);
		key = NULL; /* In case the next malloc fails, avoid a double free */
	    }

	    // Alright, we have the id and driver
	    OsinfoDevice *dev = osinfo_db_get_device(db, id);
	    if (!dev) {
	        err = -ENOENT;
		goto cleanup_error;
	    }
	    osinfo_hypervisor_add_device(hv, dev, driver);
	    free (driver);
	    driver = NULL;
	    free (id);
	    id = NULL;
        } else {
            /* Node is start of element of known name */
            char *key = NULL, *val = NULL;
            err = __osinfoProcessTag(reader, &key, &val);
            if (err != 0)
                goto cleanup_error;


            err = osinfo_entity_add_param(OSINFO_ENTITY(hv), key, val);
            free(key);
            free(val);
            if (err != 0)
                goto cleanup_error;
        }
    }

finished:
    osinfo_list_add(OSINFO_LIST(hypervisors), OSINFO_ENTITY(hv));
    g_object_unref(hv);
    return 0;
    /* At end, cursor is at end of hv node */

cleanup_error:
    g_object_unref(hv);
    return err;
}

static int __osinfoProcessDevice(OsinfoDb *db,
                                 xmlTextReaderPtr reader)
{
    /* Cursor is at start of (possibly empty) device node */

    /*
     * First, determine if device has ID or not, and if tag is empty or not.
     * The following cases can occur:
     * 1. No ID: Return invalid. Parse fails overall.
     * 2. Empty, ID: Make device with given ID and no data and return.
     * 3. Non-Empty, ID: Make device, parse data till closing tag, return.
     */

    int empty, node_type, err, ret;
    gchar* id, * key, * val;
    const gchar* name;
    OsinfoDevice *dev;
    OsinfoDeviceList *devices = osinfo_db_get_device_list(db);

    id = (gchar *)xmlTextReaderGetAttribute(reader, BAD_CAST "id");
    empty = xmlTextReaderIsEmptyElement(reader);

    if (!id)
        return -EINVAL;

    dev = osinfo_device_new(id);
    free(id);

    if (empty)
        goto finished;

    /* Current node is start of non empty device
     * While true:
     *   Advance to next node
     *   If node == end of device break
     *   If node is not element, continue
     *   If node is element:
     *     Note element name
     *     Advance to next node, ensure type is text else error
     *     Store value, advance to next node
     *     Ensure node is end of current name
     *     Store <key, val> in params list for object
     */

    for (;;) {
        /* Advance to next node */
        ret = xmlTextReaderRead(reader);
        if (ret != 1) {
            err = -EINVAL;
            goto cleanup_error;
        }

        node_type = xmlTextReaderNodeType(reader);
        name = (const gchar *)xmlTextReaderConstName(reader);

        if (node_type == -1 || !name) {
            err = -EINVAL;
            goto cleanup_error;
        }

        /* If end of device, break */
        if (node_type == END_NODE && strcmp(name, "device") == 0)
            break;

        /* If node is not start of an element, continue */
        if (node_type != ELEMENT_NODE)
            continue;

        /* Node is start of element of known name */
        err = __osinfoProcessTag(reader, &key, &val);
        if (err != 0 || !key || !val)
            goto cleanup_error;

        err = osinfo_entity_add_param(OSINFO_ENTITY(dev), key, val);
        if (err != 0)
            goto cleanup_error;

        free(key);
        free(val);
    }

finished:
    // Add dev to db
    osinfo_list_add(OSINFO_LIST(devices), OSINFO_ENTITY(dev));
    g_object_unref(dev);
    return 0;
    /* At end, cursor is at end of device node */

cleanup_error:
    free(key);
    free(val);
    g_object_unref(dev);
    return err;
}

static int __osinfoProcessFile(OsinfoDb *db,
                               xmlTextReaderPtr reader)
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

    int err, node_type, ret;
    const gchar* name;

    /* Advance to starting libosinfo tag */
    for (;;) {
        err = xmlTextReaderRead(reader);
        if (err != 1) {
            err = -EINVAL;
            goto cleanup_error;
        }

        node_type = xmlTextReaderNodeType(reader);
        if (node_type != ELEMENT_NODE)
            continue;

        name = (const gchar *)xmlTextReaderConstName(reader);
        if (strcmp(name, "libosinfo") == 0)
            break;
    }

    /* Now read and handle each tag of interest */
    for (;;) {
        /* Advance to next node */
        ret = xmlTextReaderRead(reader);
        if (ret != 1) {
            err = -EINVAL;
            goto cleanup_error;
        }

        node_type = xmlTextReaderNodeType(reader);
        name = (const gchar *)xmlTextReaderConstName(reader);

        if (node_type == -1 || !name) {
            err = -EINVAL;
            goto cleanup_error;
        }

        /* If end of libosinfo, break */
        if (node_type == END_NODE && strcmp(name, "libosinfo") == 0)
            break;

        /* If node is not start of an element, continue */
        if (node_type != ELEMENT_NODE)
            continue;

        /* Process element node */
        if (strcmp(name, "device") == 0) {
            err = __osinfoProcessDevice(db, reader);
            if (err != 0)
                goto cleanup_error;
        }
        else if (strcmp(name, "hypervisor") == 0) {
            err = __osinfoProcessHypervisor(db, reader);
            if (err != 0)
                goto cleanup_error;
        }
        else if (strcmp(name, "os") == 0) {
            err = __osinfoProcessOs(db, reader);
            if (err != 0)
                goto cleanup_error;
        }
        else {
            err = -EINVAL;
            goto cleanup_error;
        }
    }

    /* And we are done, successfully */
    return 0;

cleanup_error:
    // Db will be unsatisfactorily initiated, caller will call unref to clean it
    return err;
}

static int osinfo_dataread_file(OsinfoDb *db,
                                const char *dir,
                                const char *filename,
				GError **err)
{
    int ret;
    xmlTextReaderPtr reader;
    char *rel_name = malloc (strlen(dir) + 1 + strlen(filename) + 1);
    if (!rel_name)
      return -errno;

    stpcpy(stpcpy(stpcpy(rel_name, dir), "/"), filename);

    reader = xmlReaderForFile(rel_name, NULL, 0);
    free(rel_name);
    if (!reader) {
        return -EINVAL;
    }
    ret = __osinfoProcessFile(db, reader);
    xmlFreeTextReader(reader);
    return ret;
}

void osinfo_dataread(OsinfoDb *db, GError **err)
{
    int ret;
    DIR* dir;
    struct dirent *dp;

    char *backingDir;
    g_object_get(G_OBJECT(db), "backing-dir", &backingDir, NULL);

    /* Initialize library and check version */
    LIBXML_TEST_VERSION

    /* Get directory with backing data. Defaults to CWD */
    if (!backingDir)
      backingDir = ".";

    /* Get XML files in directory */
    dir = opendir(backingDir);
    if (!dir) {
        g_set_error_literal(err, g_quark_from_static_string("libosinfo"), 0,
			    "unable to read backing dir");
        goto cleanup;
    }

    while ((dp=readdir(dir)) != NULL) {
        if (dp->d_type != DT_REG)
            continue;
        ret = osinfo_dataread_file(db, backingDir, dp->d_name, err);
        if (ret != 0)
            break;
    }
    closedir(dir);
    if (!*err)
        __osinfoFixObjLinks(db, err);

cleanup:
    xmlCleanupParser();
    g_free(backingDir);
}

#else
void osinfo_dataread(OsinfoDb *db, GError **err)
{
    g_set_error_literal(err, g_quark_from_static_string("libosinfo"), 0,
			"xml loading not available");
}
#endif
