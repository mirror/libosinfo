
SUBDIRS = devices oses hypervisors

EXTRA_DIST = usb.ids pci.ids 95-osinfo.rules.in

databasedir = $(pkgdatadir)/data/
database_DATA = usb.ids pci.ids

if WITH_UDEV
BUILT_SOURCES = 95-osinfo.rules

95-osinfo.rules: 95-osinfo.rules.in
	sed 's,\@bindir\@,$(exec_prefix)/bin,' $< > $@

# Now copy the rules file to where it needs to be but don't worry if it fails
# since it can easily fail (e.g if `make install` is run unprivileged) and its
# not a big deal if it does.
install-data-hook: 95-osinfo.rules
	-cp 95-osinfo.rules $(DESTDIR)/lib/udev/rules.d/

endif

CLEANFILES = usb.ids pci.ids

usb.ids:
	wget -q -O $@ http://www.linux-usb.org/usb.ids

pci.ids:
	wget -q -O $@ http://pciids.sourceforge.net/v2.2/pci.ids
