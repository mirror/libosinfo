
AM_CPPFLAGS = \
	-I$(top_srcdir) \
	$(LIBXML_CFLAGS) \
	$(GOBJECT_CFLAGS)
libosinfo_la_LDFLAGS = \
	$(LIBXML_LIBS) \
	$(GOBJECT_LIBS)

lib_LTLIBRARIES = libosinfo.la

libosinfo_includedir = $(includedir)/osinfo-1.0/osinfo

libosinfo_include_HEADERS = 		\
  osinfo_db.h		\
  osinfo_device.h		\
  osinfo_devicelist.h		\
  osinfo_entity.h		\
  osinfo_filter.h		\
  osinfo_hypervisor.h		\
  osinfo_hypervisorlist.h		\
  osinfo_list.h		\
  osinfo_os.h		\
  osinfo_oslist.h

libosinfo_la_SOURCES =		\
  osinfo_common.h		\
  osinfo_common.c		\
  osinfo_dataread.c		\
  osinfo_device.c		\
  osinfo_devicelist.c		\
  osinfo_entity.c		\
  osinfo_filter.c		\
  osinfo_hypervisor.c	\
  osinfo_hypervisorlist.c	\
  osinfo_list.c	\
  osinfo_oslist.c	\
  osinfo_db.c			\
  osinfo_os.c

