/*
 * Copyright (C) 2012 Red Hat, Inc
 *
 * osinfo-query: query the contents of the database
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Authors:
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <config.h>

#include <glib.h>
#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <glib/gi18n.h>

#include <osinfo/osinfo.h>


struct OsinfoLabel {
    const gchar *prop;
    const gchar *label;
    gboolean enabled;
    gsize width;
};

typedef OsinfoList * (*osinfo_list_func)(OsinfoDb *db);

struct OsinfoType {
    const gchar *name;
    osinfo_list_func listFunc;
    GType entityType;
    GType filterType;
    GType listType;
    struct OsinfoLabel *labels;
};

static struct OsinfoLabel os_labels[] = {
    { OSINFO_PRODUCT_PROP_SHORT_ID,
      N_("Short ID"), TRUE, 20 },
    { OSINFO_PRODUCT_PROP_NAME,
      N_("Name"), TRUE, 50 },
    { OSINFO_PRODUCT_PROP_VERSION,
      N_("Version"), TRUE, 8 },
    { OSINFO_OS_PROP_FAMILY,
      N_("Family"), FALSE, 12 },
    { OSINFO_OS_PROP_DISTRO,
      N_("Distro"), FALSE, 12 },
    { OSINFO_PRODUCT_PROP_VENDOR,
      N_("Vendor"), FALSE, 25 },
    { OSINFO_PRODUCT_PROP_RELEASE_DATE,
      N_("Release date"), FALSE, 12 },
    { OSINFO_PRODUCT_PROP_EOL_DATE,
      N_("End of life"), FALSE, 12 },
    { OSINFO_PRODUCT_PROP_CODENAME,
      N_("Code name"), FALSE, 10 },
    { OSINFO_ENTITY_PROP_ID,
      N_("ID"), TRUE, 40 },
    { NULL, NULL, 0, 0},
};

static struct OsinfoLabel platform_labels[] = {
    { OSINFO_PRODUCT_PROP_SHORT_ID,
      N_("Short ID"), TRUE, 20 },
    { OSINFO_PRODUCT_PROP_NAME,
      N_("Name"), TRUE, 50 },
    { OSINFO_PRODUCT_PROP_VERSION,
      N_("Version"), TRUE, 8 },
    { OSINFO_PRODUCT_PROP_VENDOR,
      N_("Vendor"), TRUE, 25 },
    { OSINFO_PRODUCT_PROP_RELEASE_DATE,
      N_("Release date"), FALSE, 12 },
    { OSINFO_PRODUCT_PROP_EOL_DATE,
      N_("End of life"), FALSE, 12 },
    { OSINFO_PRODUCT_PROP_CODENAME,
      N_("Code name"), FALSE, 10 },
    { OSINFO_ENTITY_PROP_ID,
      N_("ID"), TRUE, 40 },
    { NULL, NULL, 0, 0 },
};

static struct OsinfoLabel device_labels[] = {
    { OSINFO_DEVICE_PROP_VENDOR,
      N_("Vendor"), TRUE, 20 },
    { OSINFO_DEVICE_PROP_VENDOR_ID,
      N_("Vendor ID"), TRUE, 12 },
    { OSINFO_DEVICE_PROP_PRODUCT,
      N_("Product"), TRUE, 20 },
    { OSINFO_DEVICE_PROP_PRODUCT_ID,
      N_("Product ID"), TRUE, 12 },
    { OSINFO_PRODUCT_PROP_NAME,
      N_("Name"), TRUE, 14 },
    { OSINFO_DEVICE_PROP_CLASS,
      N_("Class"), TRUE, 15 },
    { OSINFO_DEVICE_PROP_BUS_TYPE,
      N_("Bus"), TRUE, 8 },
    { OSINFO_ENTITY_PROP_ID,
      N_("ID"), TRUE, 40 },
    { NULL, NULL, 0, 0 },
};

static struct OsinfoLabel deployment_labels[] = {
    { OSINFO_ENTITY_PROP_ID,
      N_("ID"), TRUE, 40 },
    { NULL, NULL, 0, 0 },
};


static gboolean toggle_fields(struct OsinfoLabel *labels,
                              const gchar *fieldStr,
                              GError **error)
{
    gboolean ret = FALSE;
    gchar **fields;
    gsize i, j;

    if (!fieldStr)
        return TRUE;

    fields = g_strsplit(fieldStr, ",", 0);

    for (j = 0 ; labels[j].prop ; j++) {
        labels[j].enabled = FALSE;
    }

    for (i = 0 ; fields[i] != NULL ; i++) {
        gboolean found = FALSE;
        for (j = 0 ; labels[j].prop ; j++) {
            if (g_str_equal(fields[i], labels[j].prop)) {
                labels[j].enabled = TRUE;
                found = TRUE;
            }
        }
        if (!found) {
            g_set_error(error, 0, 0,
                        _("Unknown property name %s"), fields[i]);
            goto cleanup;
        }
    }


    ret = TRUE;

 cleanup:
    g_strfreev(fields);
    return ret;
}

static gboolean build_filter(struct OsinfoLabel *labels,
                             OsinfoFilter *filter,
                             gint argc, char **argv,
                             GError **error)
{
    gboolean ret = FALSE;
    gsize i, j;

    for (i = 0 ; i < argc ; i++) {
        const gchar *tmp = strchr(argv[i], '=');
        if (!tmp) {
            g_set_error(error, 0, 0, "%s", _("Syntax error in condition, expecting KEY=VALUE"));
            goto cleanup;
        }
        gchar *key = g_strndup(argv[i], tmp-argv[i]);
        gchar *val = g_strdup(tmp+1);
        gboolean found = FALSE;

        for (j = 0 ; labels[j].prop != NULL ; j++) {
            if (g_str_equal(key, labels[j].prop))
                found = TRUE;
        }

        if (!found) {
            g_set_error(error, 0, 0,
                        _("Unknown property name %s"), key);
            goto cleanup;
        }

        osinfo_filter_add_constraint(filter, key, val);
        g_free(key);
        g_free(val);
    }

    ret = TRUE;
 cleanup:
    return ret;
}


static gint sort_entity(gconstpointer a,
                        gconstpointer b,
                        gpointer data)
{
    OsinfoEntity *entityA = OSINFO_ENTITY(a);
    OsinfoEntity *entityB = OSINFO_ENTITY(b);
    gchar *key = data;
    const gchar *valA;
    const gchar *valB;

    valA = osinfo_entity_get_param_value(entityA, key);
    valB = osinfo_entity_get_param_value(entityB, key);

    if (!valA && !valB)
        return 0;

    if (!valA && valB)
        return 1;

    if (valA && !valB)
        return 1;

    return strcmp(valA, valB);
}


static gboolean print_entity_text(OsinfoEntity *entity,
                                  const struct OsinfoLabel *labels)
{
    gsize i;
    gboolean first = TRUE;
    for (i = 0 ; labels[i].prop != NULL ; i++) {
        gsize pad;
        gchar *padstr;
        const gchar *val = osinfo_entity_get_param_value(entity, labels[i].prop);
        if (!labels[i].enabled)
            continue;

        if (first)
            g_print(" ");
        else
            g_print(" | ");
        first = FALSE;

        if (val && (strlen(val) > labels[i].width))
            pad = 0;
        else
            pad = labels[i].width - (val ? strlen(val) : 0);

        padstr = g_new0(gchar, pad+1);
        padstr[pad] = '\0';

        if (pad)
            memset(padstr, ' ', pad);

        g_print("%s%s",
                val ? val : "", padstr);
        g_free(padstr);
    }
    g_print("\n");

    return FALSE;
}

static gboolean print_results_text(OsinfoList *list,
                                   const struct OsinfoLabel *labels,
                                   const gchar *sortKey)
{
    gboolean ret = FALSE;
    GList *entities = osinfo_list_get_elements(list);
    GList *tmp;
    gsize i;
    gboolean first = TRUE;

    tmp = entities = g_list_sort_with_data(entities, sort_entity,
                                           (gchar*)(sortKey ? sortKey :
                                                    labels[0].prop));

    for (i = 0 ; labels[i].prop != NULL ; i++) {
        gsize pad;
        gchar *padstr;
        if (!labels[i].enabled)
            continue;

        if (first)
            g_print(" ");
        else
            g_print(" | ");
        first = FALSE;

        if (strlen(gettext(labels[i].label)) > labels[i].width)
            pad = 0;
        else
            pad = labels[i].width - strlen(gettext(labels[i].label));

        padstr = g_new0(gchar, pad+1);
        padstr[pad] = '\0';

        if (pad)
            memset(padstr, ' ', pad);

        g_print("%s%s",
                gettext(labels[i].label), padstr);
        g_free(padstr);
    }
    g_print("\n");

    first = TRUE;
    for (i = 0 ; labels[i].prop != NULL ; i++) {
        gchar *padstr;
        if (!labels[i].enabled)
            continue;

        if (first)
            g_print("-");
        else
            g_print("-+-");
        first = FALSE;

        padstr = g_new0(gchar, labels[i].width+1);
        memset(padstr, '-', labels[i].width);
        padstr[labels[i].width] = '\0';

        g_print("%s", padstr);
        g_free(padstr);
    }
    g_print("\n");

    while (tmp) {
        OsinfoEntity *entity = OSINFO_ENTITY(tmp->data);

        print_entity_text(entity, labels);
        tmp = tmp->next;
    }


    g_list_free(entities);
    ret = TRUE;
    // cleanup:
    return ret;
}

gint main(gint argc, gchar **argv)
{
    GOptionContext *context;
    GError *error = NULL;
    gint ret = EXIT_FAILURE;
    const gchar *type = NULL;
    OsinfoLoader *loader = NULL;
    OsinfoDb *db = NULL;
    OsinfoList *entities = NULL;
    OsinfoFilter *filter = NULL;
    OsinfoList *results = NULL;
    struct OsinfoLabel *labels = NULL;
    gsize i;
    const gchar *sortKey = NULL;
    const gchar *fields = NULL;

    setlocale(LC_ALL, "");
    textdomain (GETTEXT_PACKAGE);
    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

    g_type_init();

    struct OsinfoType types[] = {
        { "os",
          (osinfo_list_func)osinfo_db_get_os_list,
          OSINFO_TYPE_OS,
          OSINFO_TYPE_PRODUCTFILTER,
          OSINFO_TYPE_OSLIST,
          os_labels },
        { "platform",
          (osinfo_list_func)osinfo_db_get_platform_list,
          OSINFO_TYPE_PLATFORM,
          OSINFO_TYPE_PRODUCTFILTER,
          OSINFO_TYPE_PLATFORMLIST,
          platform_labels },
        { "device",
          (osinfo_list_func)osinfo_db_get_device_list,
          OSINFO_TYPE_DEVICE,
          OSINFO_TYPE_FILTER,
          OSINFO_TYPE_DEVICELIST,
          device_labels },
        { "deployment",
          (osinfo_list_func)osinfo_db_get_deployment_list,
          OSINFO_TYPE_DEPLOYMENT,
          OSINFO_TYPE_FILTER,
          OSINFO_TYPE_DEPLOYMENTLIST,
          deployment_labels },
    };

    GOptionEntry entries[] = {
        { "sort", 's', 0, G_OPTION_ARG_STRING, &sortKey,
          _("Sort column"), NULL },
        { "fields", 'f', 0, G_OPTION_ARG_STRING, &fields,
          _("Display fields"), NULL },
        { NULL, 0, 0, 0, NULL, NULL, NULL }
    };


    context = g_option_context_new(_("- Query the OS info database"));

    g_option_context_add_main_entries(context, entries, GETTEXT_PACKAGE);

    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_printerr(_("Error while parsing commandline options: %s\n"),
                   error->message);
        g_printerr("%s\n", g_option_context_get_help(context, FALSE, NULL));
        goto error;
    }

    if (argc < 2) {
        g_printerr(_("Missing data type parameter\n"));
        goto error;
    }

    type = argv[1];

    loader = osinfo_loader_new();
    osinfo_loader_process_default_path(loader, &error);
    if (error != NULL) {
        g_printerr(_("Error loading OS data: %s\n"), error->message);
        goto error;
    }

    db = osinfo_loader_get_db(loader);


    for (i = 0 ; i < (sizeof(types)/sizeof(types[0])) ; i++) {
        if (g_str_equal(types[i].name, type)) {
            entities = types[i].listFunc(db);
            filter = g_object_new(types[i].filterType, NULL);
            results = g_object_new(types[i].listType,
                                   "element-type", types[i].entityType,
                                   NULL);
            labels = types[i].labels;
        }
    }

    if (!entities) {
        g_printerr(_("Unknown type '%s' requested\n"), type);
        goto error;
    }

    if (!build_filter(labels, filter, argc-2, argv+2, &error)) {
        g_printerr(_("Unable to construct filter: %s\n"), error->message);
        goto error;
    }

    if (!toggle_fields(labels, fields, &error)) {
        g_printerr(_("Unable to set field visibility: %s\n"), error->message);
        goto error;
    }

    osinfo_list_add_filtered(results, entities, filter);

    print_results_text(results, labels, sortKey);

    ret = EXIT_SUCCESS;

 error:
    g_clear_error(&error);
    g_option_context_free(context);

    if (entities)
        g_object_unref(entities);
    if (filter)
        g_object_unref(filter);
    if (results)
        g_object_unref(results);
    if (loader)
        g_object_unref(loader);

    return ret;
}

/*
=pod

=head1 NAME

osinfo-query - Query information in the database

=head1 SYNOPSIS

osinfo-query [OPTIONS...] TYPE [CONDITION-1 [CONDITION-2 ...]]

=head1 DESCRIPTION

The C<osinfo-query> command allows extraction of information from the
database. B<TYPE> can be one of C<os>, C<platform>, C<device>, or
C<deployment>. With no conditions specified, all entities of the given
type will be listed.

  # List all operating systems
  $ osinfo-query os
   Short ID             | Name       ...
  ----------------------+-----------
   centos-6.0           | CentOS 6.0 ...
   centos-6.1           | CentOS 6.1 ...
   ...

Conditions allow filtering based on specific properties of an entity.
For example, to filter only distros from the Fedora Project, use

  # List all operating systems
  $ osinfo-query os vendor="Fedora Project"
   Short ID             | Name          ...
  ----------------------+--------------
   fedora1              | Fedora Core 1 ...
   fedora2              | Fedora Core 2 ...
   ...

The set of fields which are printed can be controlled using the C<--fields>
command line argument:

  # List all operating systems
  $ osinfo-query --fields=short-id,version os vendor="Fedora Project"
   Short ID             | Version
  ----------------------+----------
   fedora1              | 1
   fedora2              | 2
   ...


=head1 OPTIONS

=over 8

=item B<-s PROPERTY>, B<--sort-key PROPERTY>

Set the data sorting key. Defaults sorting the first column

=item B<-f PROPERTY1,PROPERTY2,...>, B<--fields PROPERTY1,PROPERTY2,...>

Set the visibility of properties in output

=back

=head1 PROPERTY NAMES

=head2 OS

Valid property names for the C<os> type are:

=over 4

=item B<short-id>

The short OS identifier

=item B<name>

The long OS name

=item B<version>

The OS version string

=item B<family>

The OS kernel family

=item B<vendor>

The OS vendor

=item B<release-date>

The OS release date

=item B<eol-date>

The OS end-of-life date

=item B<codename>

The OS code name

=item B<id>

The OS identifier

=back


=head2 PLATFORM

Valid property names for the C<platform> type are:

=over 4

=item B<short-id>

The short platform identifier

=item B<name>

The long platform name

=item B<version>

The platform version string

=item B<vendor>

The platform vendor

=item B<release-date>

The platform release date

=item B<eol-date>

The platform end-of-life date

=item B<codename>

The platform code name

=item B<id>

The platform identifier

=back


=head2 DEVICE

Valid property names for the C<device> type are:

=over 4

=item B<name>

The device name

=item B<product>

The device product name

=item B<product-id>

The device product ID string

=item B<vendor>

The device vendor name

=item B<vendor-id>

The device vendor ID string

=item B<class>

The device type class

=item B<bus>

The device bus type

=item B<id>

The device identifier

=back


=head2 DEPLOYMENT

Valid property names for the C<deployment> type are:

=over 4

=item B<id>

The deployment identifier

=back


=head1 EXIT STATUS

The exit status will be 0 if matching entries were found,
or 1 if not matches were found

=head1 SEE ALSO

C<osinfo-db-validate(1)>, C<osinfo-detect(1)>

=head1 AUTHORS

Daniel P. Berrange <berrange@redhat.com>

=head1 COPYRIGHT

Copyright (C) 2012 Red Hat, Inc.

=head1 LICENSE

C<osinfo-query> is distributed under the termsof the GNU LGPL v2+
license. This is free software; see the source for copying conditions.
There is NO warranty; not even for MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE

=cut
*/

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */
