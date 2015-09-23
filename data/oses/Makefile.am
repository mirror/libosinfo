
SUBDIRS = \
	apple.com \
	altlinux.org \
	centos.org \
	debian.org \
	fedoraproject.org \
	freebsd.org \
	gnome.org \
	mageia.org \
	mandriva.com \
	microsoft.com \
	netbsd.org \
	novell.com \
	openbsd.org \
	opensuse.org \
	oracle.com \
	redhat.com \
	sun.com \
	suse.com \
	ubuntu.com \
	$(NULL)

databasedir = $(pkgdatadir)/db/oses/
database_in_files =	\
	windows.xml.in  \
	$(NULL)

database_DATA = $(database_in_files:.xml.in=.xml)

@INTLTOOL_XML_RULE@

EXTRA_DIST = $(database_in_files) $(database_DATA)
