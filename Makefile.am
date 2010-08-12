
AM_CPPFLAGS = -I$(top_srcdir) $(LIBXML_CFLAGS) $(GOBJECT_CFLAGS)
libosinfo_la_LDFLAGS = $(LIBXML_LIBS) $(GOBJECT_LIBS)

lib_LTLIBRARIES = libosinfo.la

libosinfo_la_SOURCES =		\
  osinfo/osinfo_db.h		\
  osinfo/osinfo_device.h		\
  osinfo/osinfo_devicelist.h		\
  osinfo/osinfo_entity.h		\
  osinfo/osinfo_filter.h		\
  osinfo/osinfo_hypervisor.h		\
  osinfo/osinfo_hypervisorlist.h		\
  osinfo/osinfo_list.h		\
  osinfo/osinfo_os.h		\
  osinfo/osinfo_oslist.h		\
  osinfo/osinfo_common.h		\
  osinfo/osinfo_common.c		\
  osinfo/osinfo_dataread.c		\
  osinfo/osinfo_device.c		\
  osinfo/osinfo_devicelist.c		\
  osinfo/osinfo_entity.c		\
  osinfo/osinfo_filter.c		\
  osinfo/osinfo_hypervisor.c	\
  osinfo/osinfo_hypervisorlist.c	\
  osinfo/osinfo_list.c	\
  osinfo/osinfo_oslist.c	\
  osinfo/osinfo_db.c			\
  osinfo/osinfo_os.c

check_PROGRAMS =			\
  test/test-skeleton

EXTRA_DIST =				\
  data/libosinfo-dummy-data.xml

LDADD = $(lib_LTLIBRARIES)

generated_test_scripts = $(patsubst %,%.sh,$(check_PROGRAMS))
TESTS = $(generated_test_scripts)

DISTCLEANFILES = $(generated_test_scripts)

test/%.sh: Makefile
	printf '#!$(SHELL)\nexec $(*F) $(srcdir)/data\n' > $@-tmp
	chmod a+x $@-tmp
	mv $@-tmp $@

TESTS_ENVIRONMENT = \
  PATH='$(abs_top_builddir)/test$(PATH_SEPARATOR)'"$$PATH"
