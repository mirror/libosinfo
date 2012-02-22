
EXTRA_DIST = libosinfo.syms

if USE_VERSION_DEFS
LIBOSINFO_VERSION_FILE = $(builddir)/libosinfo.def
BUILT_SOURCES = $(LIBOSINFO_VERSION_FILE)

%.def: %.syms
	$(AM_V_GEN)rm -f -- $@-tmp $@ ; \
	printf 'EXPORTS\n' > $@-tmp && \
	sed -e '/^$$/d; /#/d; /:/d; /\}/d; /\*/d; /LIBOSINFO_/d; s/[ \t]*\(.*\)\;/    \1/g' $^ >> $@-tmp && \
	chmod a-w $@-tmp && \
	mv $@-tmp $@

else
LIBOSINFO_VERSION_FILE = $(srcdir)/libosinfo.syms
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libosinfo-1.0.pc

lib_LTLIBRARIES = libosinfo-1.0.la

libosinfo_1_0_la_CFLAGS = \
	$(WARN_CFLAGS) \
	$(COVERAGE_CFLAGS) \
	-I$(top_srcdir) \
	$(LIBXML_CFLAGS) \
	$(GOBJECT_CFLAGS) \
	$(GIO_CFLAGS) \
	-DDATA_DIR='"$(pkgdatadir)/data"'

libosinfo_1_0_la_LDFLAGS = \
	$(COVERAGE_LDFLAGS) \
	$(LIBXML_LIBS) \
	$(GOBJECT_LIBS) \
	$(GIO_LIBS) \
	$(VERSION_SCRIPT_FLAGS)$(LIBOSINFO_VERSION_FILE) \
        -version-info $(LIBOSINFO_VERSION_INFO) \
	$(NO_UNDEFINED_FLAGS)

libosinfo_1_0_includedir = $(includedir)/libosinfo-1.0/osinfo

libosinfo_1_0_include_HEADERS = \
  osinfo.h			\
  osinfo_db.h			\
  osinfo_loader.h		\
  osinfo_device.h		\
  osinfo_devicelink.h		\
  osinfo_devicelist.h		\
  osinfo_devicelinklist.h	\
  osinfo_devicelinkfilter.h	\
  osinfo_entity.h		\
  osinfo_filter.h		\
  osinfo_product.h		\
  osinfo_productfilter.h	\
  osinfo_productlist.h		\
  osinfo_platform.h		\
  osinfo_platformlist.h		\
  osinfo_list.h			\
  osinfo_os.h			\
  osinfo_oslist.h		\
  osinfo_deployment.h		\
  osinfo_deploymentlist.h	\
  osinfo_media.h		\
  osinfo_medialist.h		\
  osinfo_resources.h		\
  osinfo_resourceslist.h	\
  osinfo_tree.h			\
  osinfo_treelist.h		\
  $(NULL)

libosinfo_1_0_la_SOURCES =	\
  osinfo_entity.c		\
  osinfo_filter.c		\
  osinfo_list.c			\
  osinfo_device.c		\
  osinfo_devicelink.c		\
  osinfo_devicelist.c		\
  osinfo_devicelinklist.c	\
  osinfo_devicelinkfilter.c	\
  osinfo_product.c		\
  osinfo_productfilter.c	\
  osinfo_productlist.c		\
  osinfo_platform.c		\
  osinfo_platformlist.c		\
  osinfo_oslist.c		\
  osinfo_os.c			\
  osinfo_deployment.c		\
  osinfo_deploymentlist.c	\
  osinfo_media.c		\
  osinfo_medialist.c		\
  osinfo_resources.c		\
  osinfo_resourceslist.c	\
  osinfo_tree.c			\
  osinfo_treelist.c		\
  osinfo_db.c			\
  osinfo_loader.c		\
  $(NULL)

if WITH_GOBJECT_INTROSPECTION

Libosinfo-1.0.gir: libosinfo-1.0.la $(G_IR_SCANNER) Makefile.am
	$(AM_V_GEN)$(G_IR_SCANNER) --quiet \
		--namespace Libosinfo \
		--nsversion 1.0 \
		--include GObject-2.0 \
		--include Gio-2.0 \
		--include libxml2-2.0 \
		--c-include="osinfo/osinfo.h" \
		--pkg-export=libosinfo-1.0 \
		--identifier-prefix=Osinfo \
		--symbol-prefix=osinfo \
		--library=$(builddir)/libosinfo-1.0.la \
		--output $@ \
		-I$(top_srcdir) \
		-I$(srcdir) \
		--verbose \
		--pkg=gobject-2.0 \
		--pkg=gio-2.0 \
		--pkg=libxml-2.0 \
		$(libosinfo_1_0_include_HEADERS:%=$(srcdir)/%) \
		$(libosinfo_1_0_la_SOURCES:%=$(srcdir)/%)

girdir = $(datadir)/gir-1.0
gir_DATA = Libosinfo-1.0.gir

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = Libosinfo-1.0.typelib

%.typelib: %.gir
	$(AM_V_GEN)$(G_IR_COMPILER) \
		--includedir=$(builddir) \
		--includedir=$(girdir) \
		-o $@ $<

if WITH_VALA
vapidir = $(datadir)/vala/vapi
vapi_DATA = libosinfo-1.0.vapi

libosinfo-1.0.vapi: Libosinfo-1.0.gir Libosinfo-1.0.typelib
	$(AM_V_GEN)$(VAPIGEN) \
		--vapidir=. \
		--pkg gobject-2.0 \
		--pkg gio-2.0 \
		--library libosinfo-1.0 \
		Libosinfo-1.0.gir

endif # WITH_VALA

CLEANFILES = $(gir_DATA) $(typelibs_DATA) $(vapi_DATA) $(wildcard *.gcno)

endif # WITH_GOBJECT_INTROSPECTION
