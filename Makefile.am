
SUBDIRS = osinfo test data tools docs examples

EXTRA_DIST = \
  COPYING.LIB \
  libosinfo.spec \
  libosinfo.spec.in \
  mingw32-libosinfo.spec.in \
  build-aux/mktempd \
  cfg.mk \
  GNUmakefile \
  maint.mk \
  $(NULL)

DISTCHECK_CONFIGURE_FLAGS=--enable-gtk-doc

ACLOCAL_AMFLAGS = -I m4

LCOV = lcov
GENHTML = genhtml

cov: clean-cov
	mkdir $(top_builddir)/coverage
	$(LCOV) -c -o $(top_builddir)/coverage/libosinfo.info.tmp \
	  -d $(top_builddir)/osinfo
	$(LCOV) -r $(top_builddir)/coverage/libosinfo.info.tmp \
	  -o $(top_builddir)/coverage/libosinfo.info
	rm $(top_builddir)/coverage/libosinfo.info.tmp
	$(GENHTML) --show-details -t "libosinfo" -o $(top_builddir)/coverage \
	  --legend $(top_builddir)/coverage/libosinfo.info


clean-cov:
	rm -rf $(top_builddir)/coverage

dist-hook: gen-ChangeLog

# Generate the ChangeLog file (with all entries since the switch to git)
# and insert it into the directory we're about to use to create a tarball.
gen_start_date = 2009-07-04
.PHONY: gen-ChangeLog
gen-ChangeLog:
	if test -d .git; then					\
	  $(top_srcdir)/build-aux/gitlog-to-changelog		\
	    --since=$(gen_start_date) > $(distdir)/cl-t;	\
	  rm -f $(distdir)/ChangeLog;				\
	  mv $(distdir)/cl-t $(distdir)/ChangeLog;		\
	fi
