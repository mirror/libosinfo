
databasedir = $(pkgdatadir)/db/devices/
database_in_files =	\
	xen.xml.in	\
	qemu-pci.xml.in	\
	qemu-usb.xml.in	\
	qemu-ps2.xml.in	\
	virtio-pci.xml.in
	$(NULL)

database_DATA = $(database_in_files:.xml.in=.xml)

@INTLTOOL_XML_RULE@

EXTRA_DIST = $(database_in_files) $(database_DATA)
