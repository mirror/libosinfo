
DOC_MODULE=Libosinfo

DOC_MAIN_SGML_FILE=$(DOC_MODULE)-docs.xml

DOC_SOURCE_DIR=$(top_srcdir)/osinfo

SCANGOBJ_OPTIONS=

SCAN_OPTIONS=--rebuild-sections --rebuild-types

MKDB_OPTIONS=--sgml-mode --output-format=xml

MKTMPL_OPTIONS=

MKHTML_OPTIONS=

FIXXREF_OPTIONS=

HFILE_GLOB=$(top_srcdir)/osinfo/*.h
CFILE_GLOB=$(top_srcdir)/osinfo/*.c

IGNORE_HFILES=

HTML_IMAGES=

content_files=

expand_content_files=

GTKDOC_CFLAGS= \
	$(LIBXML_CFLAGS) \
	$(GOBJECT_CFLAGS)\
	$(GIO_CFLAGS)
GTKDOC_LIBS = \
	$(LIBXML_LIBS) \
	$(GOBJECT_LIBS) \
	$(GIO_LIBS) \
	$(top_builddir)/osinfo/libosinfo-1.0.la

include $(top_srcdir)/gtk-doc.make
