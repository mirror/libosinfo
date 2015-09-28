INSTALL_DATA_HOOK_DEPS =

SUBDIRS = datamaps devices oses platforms install-scripts schemas
EXTRA_DIST = usb.ids pci.ids
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


install-data-hook: $(INSTALL_DATA_HOOK_DEPS)
