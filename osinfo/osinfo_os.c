#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoOs, osinfo_os, OSINFO_TYPE_ENTITY);

#define OSINFO_OS_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_OS, OsinfoOsPrivate))

static void osinfo_os_finalize (GObject *object);

static void
osinfo_os_finalize (GObject *object)
{
    OsinfoOs *self = OSINFO_OS (object);

    g_tree_destroy (self->priv->sections);
    g_tree_destroy (self->priv->sectionsAsList);
    g_hash_table_unref(self->priv->hypervisors);
    g_tree_destroy (self->priv->relationshipsByOs);
    g_tree_destroy (self->priv->relationshipsByType);

    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_os_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_os_class_init (OsinfoOsClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_os_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoOsPrivate));
}

static void osinfo_hv_section_free(gpointer value)
{
    struct __osinfoHvSection * hvSection = value;
    if (!hvSection)
        return;
    g_tree_destroy(hvSection->sections);
    g_tree_destroy(hvSection->sectionsAsList);
    g_free(hvSection);
}

static void
osinfo_os_init (OsinfoOs *self)
{
    OsinfoOsPrivate *priv;
    self->priv = priv = OSINFO_OS_GET_PRIVATE(self);

    self->priv->sections = g_tree_new_full(__osinfoStringCompare,
                                           NULL,
                                           g_free,
                                           __osinfoFreeDeviceSection);

    self->priv->sectionsAsList = g_tree_new_full(__osinfoStringCompare,
                                                 NULL,
                                                 g_free,
                                                 __osinfoFreePtrArray);

    self->priv->relationshipsByOs = g_tree_new_full(__osinfoStringCompare,
                                                NULL,
                                                g_free,
                                                __osinfoFreeRelationship);
    self->priv->relationshipsByType = g_tree_new(__osinfoIntCompareBase);

    self->priv->hypervisors = g_hash_table_new_full(g_str_hash,
						    g_str_equal,
						    g_free,
						    osinfo_hv_section_free);
}

OsinfoOs *osinfo_os_new(const gchar *id)
{
    return g_object_new(OSINFO_TYPE_OS,
			"id", id,
			NULL);
}


static int __osinfoAddOsRelationshipByOs(OsinfoOs *self,
                                         gchar *otherOsId,
                                         osinfoRelationship rel,
                                         struct __osinfoOsLink *osLink)
{
    gboolean found;
    gpointer origKey, foundValue;
    GPtrArray *relationshipsForOs;
    gchar *otherOsIdDup = NULL;

    found = g_tree_lookup_extended(self->priv->relationshipsByOs, otherOsId, &origKey, &foundValue);
    if (!found) {
        otherOsIdDup = g_strdup(otherOsId);
        relationshipsForOs = g_ptr_array_new_with_free_func(__osinfoFreeOsLink);

        g_tree_insert(self->priv->relationshipsByOs, otherOsIdDup, relationshipsForOs);
    }
    else
        relationshipsForOs = (GPtrArray *) foundValue;

    g_ptr_array_add(relationshipsForOs, osLink);
    return 0;
}

static int __osinfoAddOsRelationshipByType(OsinfoOs *self,
                                           osinfoRelationship relshp,
                                           struct __osinfoOsLink *osLink)
{
    gboolean found;
    gpointer origKey, foundValue;
    GPtrArray *relationshipsForType;

    found = g_tree_lookup_extended(self->priv->relationshipsByType, (gpointer) relshp, &origKey, &foundValue);
    if (!found) {
        relationshipsForType = g_ptr_array_new();

        g_tree_insert(self->priv->relationshipsByType, (gpointer) relshp, relationshipsForType);
    }
    else
        relationshipsForType = (GPtrArray *) foundValue;

    g_ptr_array_add(relationshipsForType, osLink);
    return 0;
}

static void __osinfoRemoveOsLink(OsinfoOs *self,
                                 gchar *otherOsId,
                                 osinfoRelationship relshp,
                                 struct __osinfoOsLink *osLink)
{
    gboolean found;
    gpointer origKey, foundValue;
    GPtrArray *relationshipsForOs;
    GPtrArray *relationshipsForType;

    // First from by-os list
    found = g_tree_lookup_extended(self->priv->relationshipsByOs, otherOsId, &origKey, &foundValue);
    if (found) {
        relationshipsForOs = (GPtrArray *) foundValue;
        g_ptr_array_remove(relationshipsForOs, osLink);
    }

    // Now from by-relshp list
    found = g_tree_lookup_extended(self->priv->relationshipsByType, (gpointer) relshp, &origKey, &foundValue);
    if (found) {
        relationshipsForType = (GPtrArray *) foundValue;
        g_ptr_array_remove(relationshipsForType, osLink);
    }
}

int __osinfoAddOsRelationship (OsinfoOs *self, gchar *otherOsId, osinfoRelationship rel)
{
    if ( !OSINFO_IS_OS(self) || !otherOsId)
        return -EINVAL;

    struct __osinfoOsLink *osLink = NULL;
    osLink = g_new0(struct __osinfoOsLink, 1);

    osLink->subjectOs = self;
    osLink->verb = rel;

    int ret;
    ret = __osinfoAddOsRelationshipByOs(self, otherOsId, rel, osLink);
    if (ret != 0)
        goto error_free;

    ret = __osinfoAddOsRelationshipByType(self, rel, osLink);
    if (ret != 0)
        goto error_cleanup;

    return ret;

error_cleanup:
    __osinfoRemoveOsLink(self, otherOsId, rel, osLink);
error_free:
    g_free(osLink);
    return ret;
}

int __osinfoAddDeviceToSectionOs(OsinfoOs *self, gchar *section, gchar *id, gchar *driver)
{
    if( !OSINFO_IS_OS(self) || !section || !id || !driver)
        return -EINVAL;

    return __osinfoAddDeviceToSection(self->priv->sections, self->priv->sectionsAsList, section, id, driver);
}

void __osinfoClearDeviceSectionOs(OsinfoOs *self, gchar *section)
{
    if (!OSINFO_IS_OS(self) || !section)
        return;

    __osinfoClearDeviceSection(self->priv->sections, self->priv->sectionsAsList, section);
}

struct __osinfoHvSection *__osinfoAddHypervisorSectionToOs(OsinfoOs *self, gchar *hvId)
{
    if (!OSINFO_IS_OS(self) || !hvId)
        return NULL;

    gboolean found;
    gpointer origKey, foundValue;
    struct __osinfoHvSection *hvSection = NULL;
    GTree *deviceSections;
    GTree *deviceSectionsAsList;

    found = g_hash_table_lookup_extended(self->priv->hypervisors, hvId, &origKey, &foundValue);
    if (!found) {
        hvSection = g_malloc(sizeof(*hvSection));
        deviceSections = g_tree_new_full(__osinfoStringCompare,
                                        NULL,
                                        g_free,
                                        __osinfoFreeDeviceSection);


        deviceSectionsAsList = g_tree_new_full(__osinfoStringCompare,
                                               NULL,
                                               g_free,
                                               __osinfoFreePtrArray);

        hvSection->os = self;
        // Will set hv link later
        hvSection->sections = deviceSections;
        hvSection->sectionsAsList = deviceSectionsAsList;

        g_hash_table_insert(self->priv->hypervisors,
			    g_strdup(hvId), hvSection);
        return hvSection;
    }
    else
        return (struct __osinfoHvSection *) foundValue;
}

void __osinfoRemoveHvSectionFromOs(OsinfoOs *self, gchar *hvId)
{
    g_hash_table_remove(self->priv->hypervisors, hvId);
}

OsinfoDevice *osinfo_os_get_preferred_device(OsinfoOs *self, OsinfoHypervisor *hv, gchar *devType, OsinfoFilter *filter)
{
    g_return_val_if_fail(OSINFO_IS_OS(self), NULL);
    g_return_val_if_fail(OSINFO_IS_HYPERVISOR(hv), NULL);
    g_return_val_if_fail(OSINFO_IS_FILTER(filter), NULL);
    g_return_val_if_fail(devType != NULL, NULL);
    // Check if device type info present for <os,hv>, else return NULL.

    GPtrArray *sectionList = NULL;
    if (hv) {
        // Check if hypervisor specific info present for Os, else return NULL.
        struct __osinfoHvSection *hvSection = NULL;
        hvSection = g_hash_table_lookup(self->priv->hypervisors, (OSINFO_ENTITY(hv))->priv->id);
        if (!hvSection)
            return NULL;

        sectionList = g_tree_lookup(hvSection->sectionsAsList, devType);
        if (!sectionList)
            return NULL;
    }
    else {
        sectionList = g_tree_lookup(self->priv->sectionsAsList, devType);
        if (!sectionList)
            return NULL;
    }

    // For each device in section list, apply filter. If filter passes, return device.
    int i;
    struct __osinfoDeviceLink *deviceLink;
    for (i = 0; i < sectionList->len; i++) {
        deviceLink = g_ptr_array_index(sectionList, i);
        if (osinfo_entity_matches_filter(OSINFO_ENTITY(deviceLink->dev), filter))
	    return deviceLink->dev;
    }

    // If no devices pass filter, return NULL.
    return NULL;
}

OsinfoOsList *osinfo_os_get_related(OsinfoOs *self, osinfoRelationship relshp)
{
    g_return_val_if_fail(OSINFO_IS_OS(self), NULL);

    // Create our list
    OsinfoOsList *newList = osinfo_oslist_new();

    GPtrArray *relatedOses = NULL;
    relatedOses = g_tree_lookup(self->priv->relationshipsByType, (gpointer) relshp);
    if (relatedOses) {
        int i, len;
        len = relatedOses->len;
        for (i = 0; i < len; i++) {
            struct __osinfoOsLink *osLink = g_ptr_array_index(relatedOses, i);
            osinfo_list_add(OSINFO_LIST (newList), OSINFO_ENTITY (osLink->directObjectOs));
        }
    }

    return newList;
}

OsinfoDeviceList *osinfo_os_get_devices(OsinfoOs *self, OsinfoHypervisor *hv, gchar *devType, OsinfoFilter *filter)
{
    g_return_val_if_fail(OSINFO_IS_OS(self), NULL);
    g_return_val_if_fail(OSINFO_IS_HYPERVISOR(hv), NULL);
    g_return_val_if_fail(OSINFO_IS_FILTER(filter), NULL);
    g_return_val_if_fail(devType != NULL, NULL);

    GPtrArray *sectionList = NULL;

    // Create our device list
    OsinfoDeviceList *newList = osinfo_devicelist_new();

    if (hv) {
        struct __osinfoHvSection *hvSection = NULL;
        hvSection = g_hash_table_lookup(self->priv->hypervisors, (OSINFO_ENTITY(hv))->priv->id);
        if (!hvSection)
            return newList;

        sectionList = g_tree_lookup(hvSection->sectionsAsList, devType);
        if (!sectionList)
            return newList;
    }
    else {
        sectionList = g_tree_lookup(self->priv->sectionsAsList, devType);
        if (!sectionList)
            return newList;
    }

    // For each device in section list, apply filter. If filter passes, add device to list.
    int i;
    struct __osinfoDeviceLink *deviceLink;
    for (i = 0; i < sectionList->len; i++) {
        deviceLink = g_ptr_array_index(sectionList, i);
        if (osinfo_entity_matches_filter(OSINFO_ENTITY(deviceLink->dev), filter))
	    osinfo_list_add(OSINFO_LIST (newList), OSINFO_ENTITY (deviceLink->dev));
    }

    return NULL;
}
