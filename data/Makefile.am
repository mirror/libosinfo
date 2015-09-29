
SUBDIRS = datamap device os platform install-script schemas

CLEANFILES =

if LOCAL_USB_IDS
usb_database_DATA = usb.ids
usb_databasedir = $(pkgdatadir)/
usb.ids:
	-wget -q -O $@ http://www.linux-usb.org/usb.ids
CLEANFILES += usb.ids
endif

if LOCAL_PCI_IDS
pci_database_DATA = pci.ids
pci_databasedir = $(pkgdatadir)/
pci.ids:
	-wget -q -O $@ http://pciids.sourceforge.net/v2.2/pci.ids
CLEANFILES += pci.ids
endif
