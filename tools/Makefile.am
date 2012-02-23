AM_CFLAGS = $(GOBJECT_CFLAGS) \
	    $(GIO_CFLAGS)     \
	    $(LIBXML_CFLAGS)  \
	    -DPKGDATADIR="\"$(pkgdatadir)\"" \
	    $(WARN_CFLAGS) \
	    -I$(top_srcdir) \
            $(NULL)

bin_PROGRAMS = osinfo-detect osinfo-db-validate

man1_MANS = osinfo-db-validate.1

POD2MAN = pod2man -c "Virtualization Support" -r "$(PACKAGE)-$(VERSION)"

%.1: %.c Makefile
	$(AM_V_GEN)$(POD2MAN) $< $(srcdir)/$@

osinfo_detect_SOURCES = osinfo-detect.c
osinfo_detect_LDADD = $(GOBJECT_LIBS) 	     \
		      $(GIO_LIBS)     	     \
		      $(LIBXML_LIBS)  	     \
		      $(top_builddir)/osinfo/libosinfo-1.0.la

osinfo_db_validate_SOURCES = osinfo-db-validate.c
osinfo_db_validate_LDADD = $(GOBJECT_LIBS)   \
		      $(GIO_LIBS)     	     \
		      $(LIBXML_LIBS)  	     \
		      $(top_builddir)/osinfo/libosinfo-1.0.la

