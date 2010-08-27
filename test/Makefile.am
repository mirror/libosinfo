
check_PROGRAMS = \
	test-entity \
	test-device \
	test-list \
	test-devicelist \
	test-filter \
	test-os \
	test-oslist \
	test-osfilter \
	test-hypervisor \
	test-hypervisorlist \
	test-db \
	test-loader

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

test_os_LDADD = $(COMMON_LDADD)
test_os_CFLAGS = $(COMMON_CFLAGS)
test_os_SOURCES = test-os.c

test_osfilter_LDADD = $(COMMON_LDADD)
test_osfilter_CFLAGS = $(COMMON_CFLAGS)
test_osfilter_SOURCES = test-osfilter.c

test_hypervisor_LDADD = $(COMMON_LDADD)
test_hypervisor_CFLAGS = $(COMMON_CFLAGS)
test_hypervisor_SOURCES = test-hypervisor.c

test_list_LDADD = $(COMMON_LDADD)
test_list_CFLAGS = $(COMMON_CFLAGS)
test_list_SOURCES = test-list.c

test_devicelist_LDADD = $(COMMON_LDADD)
test_devicelist_CFLAGS = $(COMMON_CFLAGS)
test_devicelist_SOURCES = test-devicelist.c

test_hypervisorlist_LDADD = $(COMMON_LDADD)
test_hypervisorlist_CFLAGS = $(COMMON_CFLAGS)
test_hypervisorlist_SOURCES = test-hypervisorlist.c

test_oslist_LDADD = $(COMMON_LDADD)
test_oslist_CFLAGS = $(COMMON_CFLAGS)
test_oslist_SOURCES = test-oslist.c

test_db_LDADD = $(COMMON_LDADD)
test_db_CFLAGS = $(COMMON_CFLAGS)
test_db_SOURCES = test-db.c

test_loader_LDADD = $(COMMON_LDADD)
test_loader_CFLAGS = $(COMMON_CFLAGS)
test_loader_SOURCES = test-loader.c

TESTS = $(check_PROGRAMS)

TESTS_ENVIRONMENT = \
	LC_ALL=C \
	G_SLICE=always-malloc G_DEBUG=gc-friendly \
	$(VG)

valgrind:
	$(MAKE) check VG="libtool --mode=execute valgrind --quiet --tool=memcheck --leak-check=full --leak-resolution=high --num-callers=20  --suppressions=osinfo.suppression"
