INSTALL_DATA_HOOK_DEPS =

SUBDIRS = datamaps devices oses hypervisors install-scripts schemas
EXTRA_DIST = usb.ids pci.ids 95-osinfo.rules.in
CLEANFILES = usb.ids pci.ids

if USE_INTERNAL_USB_IDS
usb_database_DATA = usb.ids
usb_databasedir = $(pkgdatadir)/db/
usb.ids:
	-wget -q -O $@ http://www.linux-usb.org/usb.ids
else
usb_ids_install:
	($(MKDIR_P) $(DESTDIR)$(pkgdatadir)/db && \
	 cd $(DESTDIR)$(pkgdatadir)/db/ && \
	 rm -f usb.ids && \
	 $(LN_S) $(USB_IDS) usb.ids)
INSTALL_DATA_HOOK_DEPS += usb_ids_install
endif

if USE_INTERNAL_PCI_IDS
pci_database_DATA = pci.ids
pci_databasedir = $(pkgdatadir)/db/
pci.ids:
	-wget -q -O $@ http://pciids.sourceforge.net/v2.2/pci.ids
else
pci_ids_install:
	($(MKDIR_P) $(DESTDIR)$(pkgdatadir)/db && \
	 cd $(DESTDIR)$(pkgdatadir)/db/ && \
	 rm -f pci.ids && \
	 $(LN_S) $(PCI_IDS) pci.ids)
INSTALL_DATA_HOOK_DEPS += pci_ids_install
endif

if WITH_UDEV
BUILT_SOURCES = 95-osinfo.rules

95-osinfo.rules: 95-osinfo.rules.in
	sed 's,\@bindir\@,$(exec_prefix)/bin,' $< > $@

95-osinfo.rules-install: 95-osinfo.rules
	$(MKDIR_P) $(DESTDIR)$(UDEV_RULESDIR)
	$(INSTALL) -m 0644 95-osinfo.rules $(DESTDIR)$(UDEV_RULESDIR)

INSTALL_DATA_HOOK_DEPS += 95-osinfo.rules-install
endif

install-data-hook: $(INSTALL_DATA_HOOK_DEPS)
