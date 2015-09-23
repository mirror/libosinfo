AC_INIT([libosinfo], [0.2.13], [libosinfo@redhat.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.11.1 foreign color-tests tar-ustar])
AC_PREREQ([2.61])
AC_CONFIG_SRCDIR([osinfo/osinfo_db.c])
AC_CONFIG_HEADERS([config.h])
AC_PROG_CC
AC_PROG_LN_S

LT_INIT([shared disable-static win32-dll])

LIBOSINFO_COMPILE_WARNINGS
GNOME_MAINTAINER_MODE_DEFINES
AM_MAINTAINER_MODE([enable])

# i18 support
IT_PROG_INTLTOOL(0.40.0)

GETTEXT_PACKAGE=AC_PACKAGE_NAME
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[The name of the gettext domain])

# Autoconf 2.61a.99 and earlier don't support linking a file only
# in VPATH builds.  But since GNUmakefile is for maintainer use
# only, it does not matter if we skip the link with older autoconf.
# Automake 1.10.1 and earlier try to remove GNUmakefile in non-VPATH
# builds, so use a shell variable to bypass this.
GNUmakefile=GNUmakefile
m4_if(m4_version_compare([2.61a.100],
        m4_defn([m4_PACKAGE_VERSION])), [1], [],
      [AC_CONFIG_LINKS([$GNUmakefile:$GNUmakefile], [],
        [GNUmakefile=$GNUmakefile])])


# Use the silent-rules feature when possible.
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

PKG_CHECK_MODULES([GOBJECT], [gobject-2.0])
PKG_CHECK_MODULES([GIO], [gio-2.0])
PKG_CHECK_MODULES([SOUP], [libsoup-2.4])
PKG_CHECK_MODULES([SOUP_GNOME], [libsoup-gnome-2.4])
PKG_CHECK_MODULES([LIBXML], [libxml-2.0 >= 2.6.0])
PKG_CHECK_MODULES([LIBXSLT], [libxslt >= 1.0.0])

GTK_DOC_CHECK([1.10],[--flavour no-tmpl])

AC_ARG_ENABLE([tests],
        AS_HELP_STRING([--enable-tests], [enable test suite]),
        [], [enable_tests=yes])

if test "x$enable_tests" != "xno" ; then
  PKG_CHECK_MODULES([CHECK], [check])
fi

LIBOSINFO_MAJOR_VERSION=`echo $VERSION | awk -F. '{print $1}'`
LIBOSINFO_MINOR_VERSION=`echo $VERSION | awk -F. '{print $2}'`
LIBOSINFO_MICRO_VERSION=`echo $VERSION | awk -F. '{print $3}'`
LIBOSINFO_VERSION=$LIBOSINFO_MAJOR_VERSION.$LIBOSINFO_MINOR_VERSION.$LIBOSINFO_MICRO_VERSION$LIBOSINFO_MICRO_VERSION_SUFFIX
LIBOSINFO_VERSION_INFO=`expr $LIBOSINFO_MAJOR_VERSION + $LIBOSINFO_MINOR_VERSION`:$LIBOSINFO_MICRO_VERSION:$LIBOSINFO_MINOR_VERSION
LIBOSINFO_VERSION_NUMBER=`expr $LIBOSINFO_MAJOR_VERSION \* 1000000 + $LIBOSINFO_MINOR_VERSION \* 1000 + $LIBOSINFO_MICRO_VERSION`

AC_SUBST([LIBOSINFO_MAJOR_VERSION])
AC_SUBST([LIBOSINFO_MINOR_VERSION])
AC_SUBST([LIBOSINFO_MICRO_VERSION])
AC_SUBST([LIBOSINFO_VERSION])
AC_SUBST([LIBOSINFO_VERSION_INFO])
AC_SUBST([LIBOSINFO_VERSION_NUMBER])

NO_UNDEFINED_LDFLAGS=
VERSION_SCRIPT_FLAGS=
USE_VERSION_DEFS=0
case "$host" in
  *-*-mingw*)
    VERSION_SCRIPT_FLAGS="-Wl,"
    USE_VERSION_DEFS=1
    NO_UNDEFINED_FLAGS="-no-undefined"
    ;;

  *-*-cygwin*)
    NO_UNDEFINED_LDFLAGS="-no-undefined"
    ;;

  *)
    NO_UNDEFINED_FLAGS="-Wl,--no-undefined"
    VERSION_SCRIPT_FLAGS=-Wl,--version-script=
    `ld --help 2>&1 | grep -- --version-script >/dev/null` || \
      VERSION_SCRIPT_FLAGS="-Wl,-M -Wl,"
    ;;
esac
AC_SUBST([NO_UNDEFINED_FLAGS])
AC_SUBST([VERSION_SCRIPT_FLAGS])
AM_CONDITIONAL([USE_VERSION_DEFS], [test "$USE_VERSION_DEFS" = "1"])

with_linux=no
case $host in
  *-*-linux*) with_linux=yes ;;
esac
AM_CONDITIONAL([WITH_LINUX], [test "$with_linux" = "yes"])


GOBJECT_INTROSPECTION_CHECK([0.9.7])

AC_ARG_ENABLE([vala],
              AS_HELP_STRING([--enable-vala], [enable Vala binding generation]),
              [], [enable_vala=check])
if test "x$enable_introspection" = "xyes" ; then
        if test "x$enable_vala" != "xno" ; then
                AC_PATH_PROG(VAPIGEN, vapigen, no)
                if test "x$VAPIGEN" != "xno"; then
                        if test "x$enable_vala" == "xcheck" ; then
                                enable_vala=yes
                        fi
                        AC_SUBST(VAPIGEN)
                else
                        if test "x$enable_vala" == "xyes" ; then
                                AC_MSG_ERROR([Cannot find the "vapigen" binary in your PATH])
                        fi
                fi
        fi
fi
AM_CONDITIONAL([WITH_VALA], [test "x$enable_vala" = "xyes"])

AC_ARG_ENABLE([coverage],
        AS_HELP_STRING([--enable-coverage], [enable GCC code coverage]),
        [], [enable_coverage=no])


COVERAGE_CFLAGS=
COVERAGE_LDFLAGS=
if test "x$enable_coverage" == "xyes" ; then
  COVERAGE_CFLAGS="-fprofile-arcs -ftest-coverage"
  COVERAGE_LDFLAGS="$COVERAGE_CFLAGS"
fi
AC_SUBST(COVERAGE_CFLAGS)
AC_SUBST(COVERAGE_LDFLAGS)

# Path to the usb.ids file -- to know if we use one shipped with another
# package, or an internal file
AC_ARG_WITH(usb-ids-path,
              [AC_HELP_STRING([--with-usb-ids-path],
                              [Specify the path to usb.ids @<:@default=(internal)@:>@])],,
                              [with_usb_ids_path="\${usb_databasedir}/usb.ids"])

AM_CONDITIONAL(USE_INTERNAL_USB_IDS, test "x$with_usb_ids_path" = "x\${usb_databasedir}/usb.ids")
AC_SUBST([USB_IDS], ["$with_usb_ids_path"])

# Path to the pci.ids file -- to know if we use one shipped with another
# package, or an internal file
AC_ARG_WITH(pci-ids-path,
              [AC_HELP_STRING([--with-pci-ids-path],
                              [Specify the path to pci.ids @<:@default=(internal)@:>@])],,
                              [with_pci_ids_path="\${pci_databasedir}/pci.ids"])

AM_CONDITIONAL(USE_INTERNAL_PCI_IDS, test "x$with_pci_ids_path" = "x\${pci_databasedir}/pci.ids")
AC_SUBST([PCI_IDS], ["$with_pci_ids_path"])

# Setup GLIB_MKENUMS to use glib-mkenums even if GLib is uninstalled.
GLIB_MKENUMS=`$PKG_CONFIG --variable=glib_mkenums glib-2.0`
AC_SUBST(GLIB_MKENUMS)

AC_CONFIG_FILES([
	Makefile
	libosinfo.spec
	mingw-libosinfo.spec
	osinfo/Makefile
	osinfo/libosinfo-1.0.pc
	osinfo/osinfo_version.h
	data/Makefile
	data/datamaps/Makefile
	data/devices/Makefile
	data/platforms/Makefile
	data/install-scripts/Makefile
	data/oses/Makefile
	data/oses/altlinux.org/Makefile
	data/oses/centos.org/Makefile
	data/schemas/Makefile
	tools/Makefile
	test/Makefile
	docs/Makefile
	docs/reference/Makefile
	examples/Makefile
	po/Makefile.in
])
AC_OUTPUT
