DISTCLEANFILES =
EXTRA_DIST = libosinfo.syms \
	     osinfo_enum_types.h.template \
	     osinfo_enum_types.c.template

BUILT_SOURCES = osinfo_enum_types.c \
		osinfo_enum_types.h

if USE_VERSION_DEFS
LIBOSINFO_VERSION_FILE = $(builddir)/libosinfo.def
BUILT_SOURCES += $(LIBOSINFO_VERSION_FILE)

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
	-I$(top_builddir) \
	$(LIBXML_CFLAGS) \
	$(LIBXSLT_CFLAGS) \
	$(GOBJECT_CFLAGS) \
	$(GIO_CFLAGS) \
	-DPKG_DATA_DIR='"$(pkgdatadir)"' \
	-DSYS_CONF_DIR='"$(sysconfdir)"' \
	-DLOCALEDIR="\"$(datadir)/locale\"" \
	$(NULL)

libosinfo_1_0_la_LIBADD = \
	$(LIBXML_LIBS) \
	$(LIBXSLT_LIBS) \
	$(GOBJECT_LIBS) \
	$(GIO_LIBS)

libosinfo_1_0_la_LDFLAGS = \
	$(COVERAGE_LDFLAGS) \
	$(VERSION_SCRIPT_FLAGS)$(LIBOSINFO_VERSION_FILE) \
        -version-info $(LIBOSINFO_VERSION_INFO) \
	$(NO_UNDEFINED_FLAGS)

libosinfo_1_0_la_DEPENDENCIES = libosinfo.syms

libosinfo_1_0_includedir = $(includedir)/libosinfo-1.0/osinfo

OSINFO_HEADER_FILES =			\
  osinfo.h				\
  osinfo_avatar_format.h		\
  osinfo_db.h				\
  osinfo_loader.h			\
  osinfo_datamap.h			\
  osinfo_datamaplist.h			\
  osinfo_device.h			\
  osinfo_devicelink.h			\
  osinfo_devicelist.h			\
  osinfo_devicelinklist.h		\
  osinfo_devicelinkfilter.h		\
  osinfo_device_driver.h		\
  osinfo_device_driverlist.h		\
  osinfo_entity.h			\
  osinfo_filter.h			\
  osinfo_install_config.h		\
  osinfo_install_config_param.h		\
  osinfo_install_config_paramlist.h	\
  osinfo_install_script.h		\
  osinfo_install_scriptlist.h		\
  osinfo_product.h			\
  osinfo_productfilter.h		\
  osinfo_productlist.h			\
  osinfo_platform.h			\
  osinfo_platformlist.h			\
  osinfo_list.h				\
  osinfo_os.h				\
  osinfo_oslist.h			\
  osinfo_os_variant.h			\
  osinfo_os_variantlist.h		\
  osinfo_deployment.h			\
  osinfo_deploymentlist.h		\
  osinfo_media.h			\
  osinfo_medialist.h			\
  osinfo_resources.h			\
  osinfo_resourceslist.h		\
  osinfo_tree.h				\
  osinfo_treelist.h			\
  $(NULL)

libosinfo_1_0_include_HEADERS =		\
  $(OSINFO_HEADER_FILES)		\
  osinfo_version.h			\
  osinfo_enum_types.h			\
  $(NULL)

libosinfo_1_0_la_SOURCES =		\
  osinfo_avatar_format.c		\
  osinfo_datamap.c			\
  osinfo_datamaplist.c			\
  osinfo_entity.c			\
  osinfo_enum_types.c			\
  osinfo_filter.c			\
  osinfo_list.c				\
  osinfo_device.c			\
  osinfo_devicelink.c			\
  osinfo_devicelist.c			\
  osinfo_devicelinklist.c		\
  osinfo_devicelinkfilter.c		\
  osinfo_device_driver.c		\
  osinfo_device_driver_private.h	\
  osinfo_device_driverlist.c		\
  osinfo_install_config.c		\
  osinfo_install_config_param.c		\
  osinfo_install_config_paramlist.c	\
  osinfo_install_script.c		\
  osinfo_install_script_private.h	\
  osinfo_install_scriptlist.c		\
  osinfo_product.c			\
  osinfo_product_private.h		\
  osinfo_productfilter.c		\
  osinfo_productlist.c			\
  osinfo_platform.c			\
  osinfo_platformlist.c			\
  osinfo_oslist.c			\
  osinfo_os.c				\
  osinfo_os_variant.c			\
  osinfo_os_variantlist.c		\
  osinfo_deployment.c			\
  osinfo_deploymentlist.c		\
  osinfo_media.c			\
  osinfo_media_private.h		\
  osinfo_medialist.c			\
  osinfo_resources.c			\
  osinfo_resourceslist.c		\
  osinfo_tree.c				\
  osinfo_treelist.c			\
  osinfo_db.c				\
  osinfo_loader.c			\
  ignore-value.h			\
  $(NULL)

osinfo_enum_types.h: $(OSINFO_HEADER_FILES) osinfo_enum_types.h.template
	$(AM_V_GEN) ( $(GLIB_MKENUMS) --template $(srcdir)/osinfo_enum_types.h.template $(OSINFO_HEADER_FILES:%=$(srcdir)/%) ) > $@

osinfo_enum_types.c: $(OSINFO_HEADER_FILES) osinfo_enum_types.c.template osinfo_enum_types.h
	$(AM_V_GEN) ( $(GLIB_MKENUMS) --template $(srcdir)/osinfo_enum_types.c.template $(OSINFO_HEADER_FILES:%=$(srcdir)/%) ) > $@

DISTCLEANFILES +=				\
	osinfo_enum_types.c			\
	osinfo_enum_types.h			\
	$(NULL)

# .libs/libosinfo.so is built by libtool as a side-effect of the Makefile
# rule for libosinfo.la.  However, checking symbols relies on Linux ELF layout
if WITH_LINUX
check-symfile: libosinfo.syms libosinfo-1.0.la
	$(AM_V_GEN)$(PERL) $(srcdir)/check-symfile.pl $(srcdir)/libosinfo.syms \
	  .libs/libosinfo-1.0.so
else ! WITH_LINUX
check-symfile:
endif ! WITH_LINUX
check-symsorting:
	$(AM_V_GEN)$(PERL) $(srcdir)/check-symsorting.pl \
		$(srcdir) $(srcdir)/libosinfo.syms
EXTRA_DIST += check-symfile.pl check-symsorting.pl

check-local: check-symfile check-symsorting

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
CLEANFILES =

if HAVE_INTROSPECTION
Libosinfo-1.0.gir: libosinfo-1.0.la
Libosinfo_1_0_gir_INCLUDES = GObject-2.0 Gio-2.0 libxml2-2.0
Libosinfo_1_0_gir_PACKAGES = gobject-2.0 gio-2.0 libxml-2.0 libxslt
Libosinfo_1_0_gir_EXPORT_PACKAGES = libosinfo-1.0
Libosinfo_1_0_gir_LIBS = libosinfo-1.0.la
Libosinfo_1_0_gir_FILES = $(libosinfo_1_0_include_HEADERS) $(libosinfo_1_0_la_SOURCES)
Libosinfo_1_0_gir_CFLAGS = $(libosinfo_1_0_la_CFLAGS)
Libosinfo_1_0_gir_SCANNERFLAGS = --identifier-prefix=Osinfo --symbol-prefix=osinfo --c-include="osinfo/osinfo.h"
INTROSPECTION_GIRS += Libosinfo-1.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = Libosinfo-1.0.gir

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = Libosinfo-1.0.typelib

CLEANFILES += $(gir_DATA) $(typelib_DATA)

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

CLEANFILES += $(gir_DATA) $(typelibs_DATA) $(vapi_DATA) $(wildcard *.gcno)

endif # HAVE_INTROSPECTION
