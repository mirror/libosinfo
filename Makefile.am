
AM_CPPFLAGS = -I$(top_srcdir)/inc $(LIBXML_CFLAGS)
libosinfo_la_LDFLAGS = $(LIBXML_LIBS)

lib_LTLIBRARIES = libosinfo.la

libosinfo_la_SOURCES =		\
  inc/libosinfo.h		\
  inc/osi_api_methods.h		\
  inc/osi_api_types.h		\
  src/list.h			\
  src/osi_common.c		\
  src/osi_dataread.c		\
  src/osi_device.c		\
  src/osi_filter.c		\
  src/osi_hv.c			\
  src/osi_internal_types.h	\
  src/osi_lib.c			\
  src/osi_os.c

check_PROGRAMS =			\
  test/sample				\
  test/test-filter			\
  test/test-get_devices			\
  test/test-get_os			\
  test/test-manipulate_devices		\
  test/test-manipulate_hypervisor	\
  test/test-manipulate_library		\
  test/test-manipulate_os		\
  test/test-set_hypervisor		\
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
