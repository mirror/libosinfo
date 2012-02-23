/*
 * Copyright (C) 2012 Red Hat, Inc
 *
 * osinfo-validate: validate that XML file(s) follows the published schema
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
#include <libxml/relaxng.h>

#define SCHEMA PKGDATADIR "/schemas/libosinfo.rng"

static gboolean verbose = FALSE;

static const GOptionEntry entries[] = {
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, (void*)&verbose,
      "Verbose progress information", NULL, },
    { NULL, 0, 0, 0, NULL, NULL, NULL }
};


static void validate_generic_error_nop(void *userData G_GNUC_UNUSED,
                                       const char *msg G_GNUC_UNUSED,
                                       ...)
{
}

static void validate_structured_error_nop(void *userData G_GNUC_UNUSED,
                                          xmlErrorPtr error G_GNUC_UNUSED)
{
    if (error->file)
        g_printerr("%s:%d %s", error->file, error->line, error->message);
    else
        g_printerr("Schema validity error %s", error->message);
}

static xmlDocPtr parse_file(GFile *file, GError **error)
{
    xmlDocPtr doc = NULL;
    xmlParserCtxtPtr pctxt;
    gchar *data = NULL;
    gsize length;
    gchar *uri = g_file_get_uri(file);

    if (!g_file_load_contents(file, NULL, &data, &length, NULL, error))
        goto cleanup;

    if (!(pctxt = xmlNewParserCtxt())) {
        g_set_error(error, 0, 0, "%s",
                    "Unable to create libxml parser");
        goto cleanup;
    }

    if (!(doc = xmlCtxtReadDoc(pctxt, (const xmlChar*)data, uri, NULL,
                               XML_PARSE_NOENT | XML_PARSE_NONET |
                               XML_PARSE_NOWARNING))) {
        g_set_error(error, 0, 0,
                    "Unable to parse XML document %s",
                    uri);
        goto cleanup;
    }

 cleanup:
    g_free(uri);
    g_free(data);
    return doc;
}

static gboolean validate_file(xmlRelaxNGValidCtxtPtr rngValid, GFile *file, GFileInfo *info, GError **error);


static gboolean validate_file_regular(xmlRelaxNGValidCtxtPtr rngValid,
                                      GFile *file,
                                      GError **error)
{
    gboolean ret = FALSE;
    xmlDocPtr doc = NULL;
    gchar *uri = g_file_get_uri(file);

    if (!g_str_has_suffix(uri, ".xml")) {
        ret = TRUE;
        goto cleanup;
    }

    if (!(doc = parse_file(file, error)))
        goto cleanup;

    if (xmlRelaxNGValidateDoc(rngValid, doc) != 0) {
        g_set_error(error, 0, 0,
                    "Unable to validate doc %s",
                    uri);
        goto cleanup;
    }

    ret = TRUE;

 cleanup:
    //g_free(uri);
    xmlFreeDoc(doc);
    return ret;
}


static gboolean validate_file_directory(xmlRelaxNGValidCtxtPtr rngValid, GFile *file, GError **error)
{
    gboolean ret = FALSE;
    GFileEnumerator *children = NULL;
    GFileInfo *info = NULL;

    if (!(children = g_file_enumerate_children(file, "standard::*", 0, NULL, error)))
        goto cleanup;

    while ((info = g_file_enumerator_next_file(children, NULL, error))) {
        GFile *child = g_file_get_child(file, g_file_info_get_name(info));
        if (!validate_file(rngValid, child, info, error)) {
            g_object_unref(child);
            goto cleanup;
        }
        g_object_unref(child);
    }

    if (*error)
        goto cleanup;

    ret = TRUE;

 cleanup:
    g_object_unref(children);
    return ret;
}


static gboolean validate_file(xmlRelaxNGValidCtxtPtr rngValid, GFile *file, GFileInfo *info, GError **error)
{
    gboolean ret = FALSE;
    GFileInfo *thisinfo = NULL;
    gchar *uri = g_file_get_uri(file);

    if (verbose)
        g_print("Process %s\n", uri);

    if (!info) {
        if (!(thisinfo = g_file_query_info(file, "standard::*",
                                           G_FILE_QUERY_INFO_NONE,
                                           NULL, error)))
            goto cleanup;
        info = thisinfo;
    }

    if (g_file_info_get_file_type(info) == G_FILE_TYPE_DIRECTORY) {
        if (!validate_file_directory(rngValid, file, error))
            goto cleanup;
    } else if (g_file_info_get_file_type(info) == G_FILE_TYPE_REGULAR) {
        if (!validate_file_regular(rngValid, file, error))
            goto cleanup;
    } else {
        g_set_error(error, 0, 0,
                    "Unable to handle file type for %s",
                    uri);
        goto cleanup;
    }

    ret = TRUE;

 cleanup:
    g_free(uri);
    if (thisinfo)
        g_object_unref(thisinfo);
    return ret;
}


static gboolean validate_files(gint argc, gchar **argv, GError **error)
{
    xmlRelaxNGParserCtxtPtr rngParser = NULL;
    xmlRelaxNGPtr rng = NULL;
    xmlRelaxNGValidCtxtPtr rngValid = NULL;
    gboolean ret = FALSE;
    gsize i;

    xmlSetGenericErrorFunc(NULL, validate_generic_error_nop);
    xmlSetStructuredErrorFunc(NULL, validate_structured_error_nop);

    rngParser = xmlRelaxNGNewParserCtxt(SCHEMA);
    if (!rngParser) {
        g_set_error(error, 0, 0,
                    "Unable to create RNG parser for %s",
                    SCHEMA);
        goto cleanup;
    }

    rng = xmlRelaxNGParse(rngParser);
    if (!rng) {
        g_set_error(error, 0, 0,
                    "Unable to parse RNG %s",
                    SCHEMA);
        goto cleanup;
    }

    rngValid = xmlRelaxNGNewValidCtxt(rng);
    if (!rngValid) {
        g_set_error(error, 0, 0,
                    "Unable to create RNG validation context %s",
                    SCHEMA);
        goto cleanup;
    }

    for (i = 0 ; i < argc ; i++) {
        GFile *file = g_file_new_for_commandline_arg(argv[i]);
        if (!validate_file(rngValid, file, NULL, error)) {
            g_object_unref(file);
            goto cleanup;
        }
        g_object_unref(file);
    }

    ret = TRUE;

 cleanup:
    xmlRelaxNGFreeValidCtxt(rngValid);
    xmlRelaxNGFreeParserCtxt(rngParser);
    xmlRelaxNGFree(rng);
    return ret;
}

gint main(gint argc, gchar **argv)
{
    GOptionContext *context;
    GError *error = NULL;
    gint ret = EXIT_FAILURE;

    g_type_init();

    context = g_option_context_new("- Validate XML documents ");

    g_option_context_add_main_entries(context, entries, NULL);

    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_printerr("Error while parsing options: %s\n", error->message);
        g_printerr("%s\n", g_option_context_get_help(context, FALSE, NULL));
	goto error;
    }

    if (!validate_files(argc - 1, argv + 1, &error)) {
        g_printerr("%s\n", error->message);
        goto error;
    }

    ret = EXIT_SUCCESS;

 error:
    g_clear_error(&error);
    g_option_context_free(context);

    return ret;
}

/*
=pod

=head1 NAME

osinfo-db-validate - Validate libosinfo XML data files

=head1 SYNOPSIS

osinfo-db-validate [OPTIONS...] LOCAL-PATH1 [LOCAL-PATH2...]

osinfo-db-validate [OPTIONS...] URI1 [URI2...]

=head1 DESCRIPTION

Check that all files (C<LOCAL-PATH1> or C<URI1>) comply with the
libosinfo XML schema. The local path may point to a directory
containing XML files, or directly to an XML file. The uris must
point directly to remote XML files

Any validation errors will be displayed on the console when
detected.

=head1 OPTIONS

=over 8

=item B<-v>, B<--verbose>

Display verbose progress information when validating files

=back

=head1 EXIT STATUS

The exit status will be 0 if all files passed validation,
or 1 if a validation error was hit.

=head1 SEE ALSO

C<xmllint(1)>

=head1 AUTHORS

Daniel P. Berrange <berrange@redhat.com>

=head1 COPYRIGHT

Copyright (C) 2012 Red Hat, Inc.

=head1 LICENSE

C<osinfo-db-validate> is distributed under the termsof the GNU LGPL v2+
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
