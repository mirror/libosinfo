
check_PROGRAMS = \
	test-entity \
	test-device \
	test-list \
	test-devicelist \
	test-filter \
	test-product \
	test-os \
	test-oslist \
	test-productfilter \
	test-platform \
	test-platformlist \
	test-db \
	test-loader \
	test-isodetect

COMMON_LDADD = \
	$(COVERAGE_LDFLAGS) \
	$(GOBJECT_LIBS) \
	$(CHECK_LIBS) \
	../osinfo/libosinfo-1.0.la
COMMON_CFLAGS = \
	$(WARN_CFLAGS) \
	$(COVERAGE_CFLAGS) \
	$(GOBJECT_CFLAGS) \
	-I$(top_srcdir) \
	-DSRCDIR="\"$(top_srcdir)\"" \
	$(CHECK_CFLAGS)

test_entity_LDADD = $(COMMON_LDADD)
test_entity_CFLAGS = $(COMMON_CFLAGS)
test_entity_SOURCES = test-entity.c

test_device_LDADD = $(COMMON_LDADD)
test_device_CFLAGS = $(COMMON_CFLAGS)
test_device_SOURCES = test-device.c

test_filter_LDADD = $(COMMON_LDADD)
test_filter_CFLAGS = $(COMMON_CFLAGS)
test_filter_SOURCES = test-filter.c

test_product_LDADD = $(COMMON_LDADD)
test_product_CFLAGS = $(COMMON_CFLAGS)
test_product_SOURCES = test-product.c

test_os_LDADD = $(COMMON_LDADD)
test_os_CFLAGS = $(COMMON_CFLAGS)
test_os_SOURCES = test-os.c

test_productfilter_LDADD = $(COMMON_LDADD)
test_productfilter_CFLAGS = $(COMMON_CFLAGS)
test_productfilter_SOURCES = test-productfilter.c

test_platform_LDADD = $(COMMON_LDADD)
test_platform_CFLAGS = $(COMMON_CFLAGS)
test_platform_SOURCES = test-platform.c

test_list_LDADD = $(COMMON_LDADD)
test_list_CFLAGS = $(COMMON_CFLAGS)
test_list_SOURCES = test-list.c

test_devicelist_LDADD = $(COMMON_LDADD)
test_devicelist_CFLAGS = $(COMMON_CFLAGS)
test_devicelist_SOURCES = test-devicelist.c

test_platformlist_LDADD = $(COMMON_LDADD)
test_platformlist_CFLAGS = $(COMMON_CFLAGS)
test_platformlist_SOURCES = test-platformlist.c

test_oslist_LDADD = $(COMMON_LDADD)
test_oslist_CFLAGS = $(COMMON_CFLAGS)
test_oslist_SOURCES = test-oslist.c

test_db_LDADD = $(COMMON_LDADD)
test_db_CFLAGS = $(COMMON_CFLAGS)
test_db_SOURCES = test-db.c

test_loader_LDADD = $(COMMON_LDADD)
test_loader_CFLAGS = $(COMMON_CFLAGS)
test_loader_SOURCES = test-loader.c

test_isodetect_LDADD = $(COMMON_LDADD)
test_isodetect_CFLAGS = $(COMMON_CFLAGS)
test_isodetect_SOURCES = test-isodetect.c

TESTS = $(check_PROGRAMS)

TESTS_ENVIRONMENT = \
	LC_ALL=C \
	G_SLICE=always-malloc G_DEBUG=gc-friendly \
	$(VG)

valgrind:
	$(MAKE) check VG="libtool --mode=execute valgrind --quiet --tool=memcheck --leak-check=full --leak-resolution=high --num-callers=20  --suppressions=osinfo.suppression"
