
SUBDIRS = devices oses hypervisors

EXTRA_DIST = usb.ids pci.ids 95-osinfo.rules.in

databasedir = $(pkgdatadir)/data/
database_DATA = usb.ids pci.ids

if WITH_UDEV
udevdir = /lib/udev/rules.d/
udev_DATA = 95-osinfo.rules
BUILT_SOURCES = 95-osinfo.rules

95-osinfo.rules: 95-osinfo.rules.in
	sed 's,\@bindir\@,$(exec_prefix)/bin,' $< > $@
endif

CLEANFILES = usb.ids pci.ids

usb.ids:
	wget -q -O $@ http://www.linux-usb.org/usb.ids

pci.ids:
	wget -q -O $@ http://pciids.sourceforge.net/v2.2/pci.ids
