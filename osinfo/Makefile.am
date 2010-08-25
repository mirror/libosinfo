
EXTRA_DIST = libosinfo.syms

if USE_VERSION_DEFS
LIBOSINFO_VERSION_FILE = $(builddir)/libosinfo.def
BUILT_SOURCES = $(LIBOSINFO_VERSION_FILE)

%.def: %.syms
	$(AM_V_GEN)rm -f -- $@-tmp $@ ; \
	printf 'EXPORTS\n' > $@-tmp && \
	sed -e '/^$$/d; /#/d; /:/d; /\}/d; /\*/d; /LIBVIRT_/d; s/[ \t]*\(.*\)\;/    \1/g' $^ >> $@-tmp && \
	chmod a-w $@-tmp && \
	mv $@-tmp $@

else
LIBOSINFO_VERSION_FILE = libosinfo.syms
endif

lib_LTLIBRARIES = libosinfo-1.0.la

libosinfo_1_0_la_CFLAGS = \
	$(WARN_CFLAGS) \
	-I$(top_srcdir) \
	$(LIBXML_CFLAGS) \
	$(GOBJECT_CFLAGS)

libosinfo_1_0_la_LDFLAGS = \
	$(LIBXML_LIBS) \
	$(GOBJECT_LIBS) \
	$(VERSION_SCRIPT_FLAGS)$(LIBOSINFO_VERSION_FILE) \
        -version-info $(LIBOSINFO_VERSION_INFO) \
	$(NO_UNDEFINED_FLAGS)

libosinfo_1_0_includedir = $(includedir)/osinfo-1.0/osinfo

libosinfo_1_0_include_HEADERS = 		\
  osinfo_db.h		\
  osinfo_device.h		\
  osinfo_devicelist.h		\
  osinfo_entity.h		\
  osinfo_filter.h		\
  osinfo_hypervisor.h		\
  osinfo_hypervisorlist.h		\
  osinfo_list.h		\
  osinfo_os.h		\
  osinfo_osfilter.h     \
  osinfo_oslist.h

libosinfo_1_0_la_SOURCES =		\
  osinfo_entity.c		\
  osinfo_filter.c		\
  osinfo_list.c			\
  osinfo_device.c		\
  osinfo_devicelist.c		\
  osinfo_hypervisor.c		\
  osinfo_hypervisorlist.c	\
  osinfo_osfilter.c		\
  osinfo_oslist.c		\
  osinfo_os.c			\
  osinfo_dataread.c		\
  osinfo_db.c

