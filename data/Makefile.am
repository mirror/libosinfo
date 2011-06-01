
SUBDIRS = devices oses hypervisors

EXTRA_DIST = usb.ids pci.ids

databasedir = $(pkgdatadir)/data/
database_DATA = usb.ids pci.ids

CLEANFILES = usb.ids pci.ids

usb.ids:
	wget -q -O $@ http://www.linux-usb.org/usb.ids

pci.ids:
	wget -q -O $@ http://pciids.sourceforge.net/v2.2/pci.ids
