
check_PROGRAMS =			\
	test-skeleton


generated_test_scripts = $(patsubst %,%.sh,$(check_PROGRAMS))
TESTS = $(generated_test_scripts)

test_skeleton_LDADD = ../osinfo/libosinfo.la
test_skeleton_SOURCES = test-skeleton.c
test_skeleton_CFLAGS = \
	-I$(top_srcdir)/ \
	$(GOBJECT_CFLAGS)

DISTCLEANFILES = $(generated_test_scripts)

%.sh: Makefile
	printf '#!$(SHELL)\nexec $(*F) $(srcdir)/data\n' > $@-tmp
	chmod a+x $@-tmp
	mv $@-tmp $@

TESTS_ENVIRONMENT = \
  PATH='$(abs_top_builddir)/test$(PATH_SEPARATOR)'"$$PATH"
