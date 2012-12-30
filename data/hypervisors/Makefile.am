
databasedir = $(pkgdatadir)/db/hypervisors/
database_in_files = kvm.xml.in qemu.xml.in rhel-xen.xml.in xen.xml.in

database_DATA = $(database_in_files:.xml.in=.xml)

@INTLTOOL_XML_RULE@

EXTRA_DIST = $(database_in_files) $(database_DATA)
