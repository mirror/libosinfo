
SUBDIRS = \
	apple.com \
	altlinux.org \
	centos.org \
	debian.org \
	fedoraproject.org \
	freebsd.org \
	gnome.org \
	mageia.org \
	microsoft.com \
	$(NULL)

databasedir = $(pkgdatadir)/db/oses/
database_in_files =	\
	mandriva.xml.in \
	mandrake.xml.in \
	netbsd.xml.in	\
	netware.xml.in  \
	openbsd.xml.in  \
	opensuse.xml.in \
	rhl.xml.in      \
	rhel.xml.in     \
	solaris.xml.in  \
	suse.xml.in     \
	ubuntu.xml.in   \
	windows.xml.in  \
	$(NULL)

database_DATA = $(database_in_files:.xml.in=.xml)

@INTLTOOL_XML_RULE@

EXTRA_DIST = $(database_in_files) $(database_DATA)
