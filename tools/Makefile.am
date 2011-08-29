AM_CFLAGS = $(GOBJECT_CFLAGS) \
	    $(GIO_CFLAGS)     \
	    $(LIBXML_CFLAGS)  \
	    -I$(top_srcdir)

bin_PROGRAMS = osinfo-detect

osinfo_detect_SOURCES = osinfo-detect.c

osinfo_detect_LDADD = $(GOBJECT_LIBS) 	     \
		      $(GIO_LIBS)     	     \
		      $(LIBXML_LIBS)  	     \
		      $(top_builddir)/osinfo/libosinfo-1.0.la

