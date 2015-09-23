# Generic rules that work for any subdir

databasedir := $(pkgdatadir)/db/$(abs_srcdir:$(abs_top_srcdir)/data/%=%)
database_SRC = $(wildcard $(srcdir)/*.xml.in)
database_DATA = $(database_SRC:$(srcdir)/%.xml.in=%.xml)

@INTLTOOL_XML_RULE@

EXTRA_DIST = $(database_SRC)

CLEANFILES = $(database_DATA)
