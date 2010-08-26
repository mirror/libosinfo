
SUBDIRS = osinfo test

EXTRA_DIST = \
  COPYING.LIB \
  data/libosinfo-dummy-data.xml \
  libosinfo.spec.in \
  mingw32-libosinfo.spec.in


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