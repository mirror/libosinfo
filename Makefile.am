
AM_CPPFLAGS = -I$(top_srcdir)/inc -I$(top_srcdir)/api $(LIBXML_CFLAGS) $(GOBJECT_CFLAGS)
libosinfo_la_LDFLAGS = $(LIBXML_LIBS) $(GOBJECT_LIBS)

lib_LTLIBRARIES = libosinfo.la

libosinfo_la_SOURCES =		\
  api/osinfo_db.h		\
  api/osinfo_device.h		\
  api/osinfo_devicelist.h		\
  api/osinfo_entity.h		\
  api/osinfo_filter.h		\
  api/osinfo_hypervisor.h		\
  api/osinfo_hypervisorlist.h		\
  api/osinfo_list.h		\
  api/osinfo_os.h		\
  api/osinfo_oslist.h		\
  inc/osinfo_common.h		\
  src/osinfo_common.c		\
  src/osinfo_dataread.c		\
  src/osinfo_device.c		\
  src/osinfo_devicelist.c		\
  src/osinfo_entity.c		\
  src/osinfo_filter.c		\
  src/osinfo_hypervisor.c	\
  src/osinfo_hypervisorlist.c	\
  src/osinfo_list.c	\
  src/osinfo_oslist.c	\
  src/osinfo_db.c			\
  src/osinfo_os.c

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
