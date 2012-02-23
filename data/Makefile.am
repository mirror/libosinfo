
SUBDIRS = devices oses hypervisors schemas

EXTRA_DIST = usb.ids pci.ids 95-osinfo.rules.in

databasedir = $(pkgdatadir)/data/
database_DATA = usb.ids pci.ids

if WITH_UDEV
BUILT_SOURCES = 95-osinfo.rules

95-osinfo.rules: 95-osinfo.rules.in
	sed 's,\@bindir\@,$(exec_prefix)/bin,' $< > $@

install-data-hook: 95-osinfo.rules
	mkdir -p $(DESTDIR)$(UDEV_RULESDIR)
	$(INSTALL) -m 0644 95-osinfo.rules $(DESTDIR)$(UDEV_RULESDIR)

endif

CLEANFILES = usb.ids pci.ids

usb.ids:
	-wget -q -O $@ http://www.linux-usb.org/usb.ids

pci.ids:
	-wget -q -O $@ http://pciids.sourceforge.net/v2.2/pci.ids
