AC_INIT([libosinfo], [0.2.0], [virt-tools-list@redhat.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.11.1 foreign color-tests tar-ustar])
AC_PREREQ([2.61])
AC_CONFIG_SRCDIR([osinfo/osinfo_db.c])
AC_CONFIG_HEADERS([config.h])
AC_PROG_LIBTOOL
AC_PROG_CC

LIBOSINFO_COMPILE_WARNINGS
GNOME_MAINTAINER_MODE_DEFINES
AM_MAINTAINER_MODE([enable])

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

AC_ARG_ENABLE([introspection],
        AS_HELP_STRING([--enable-introspection], [enable GObject introspection]),
        [], [enable_introspection=check])

if test "x$enable_introspection" != "xno" ; then
        PKG_CHECK_MODULES([GOBJECT_INTROSPECTION],
                          [gobject-introspection-1.0 >= 0.9.0],
                          [enable_introspection=yes],
                          [
                             if test "x$enable_introspection" = "xcheck"; then
                               enable_introspection=no
                             else
                               AC_MSG_ERROR([gobject-introspection is not available])
                             fi
                          ])
        if test "x$enable_introspection" = "xyes" ; then
          AC_DEFINE([WITH_GOBJECT_INTROSPECTION], [1], [enable GObject introspection support])
          AC_SUBST(GOBJECT_INTROSPECTION_CFLAGS)
          AC_SUBST(GOBJECT_INTROSPECTION_LIBS)
          AC_SUBST([G_IR_SCANNER], [$($PKG_CONFIG --variable=g_ir_scanner gobject-introspection-1.0)])
          AC_SUBST([G_IR_COMPILER], [$($PKG_CONFIG --variable=g_ir_compiler gobject-introspection-1.0)])
        fi
fi
AM_CONDITIONAL([WITH_GOBJECT_INTROSPECTION], [test "x$enable_introspection" = "xyes"])

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

AC_ARG_ENABLE([udev],
        AS_HELP_STRING([--enable-udev], [enable Udev support]),
        [], [enable_udev=no])
AM_CONDITIONAL([WITH_UDEV], [test "x$enable_udev" = "xyes"])

AC_ARG_WITH([udev-rulesdir],
             [AS_HELP_STRING([--with-udev-rulesdir],[UDev rules directory])],
             [], [])

if test "x$with_udev_rulesdir" = "x"; then
        if test "x$prefix" = "x/usr" ; then
                with_udev_rulesdir=/lib/udev/rules.d
        else
                with_udev_rulesdir=$prefix/lib/udev/rules.d
        fi
fi

UDEV_RULESDIR="$with_udev_rulesdir"
AC_SUBST(UDEV_RULESDIR)

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


AC_CONFIG_FILES([
	Makefile
	libosinfo.spec
	mingw-libosinfo.spec
	osinfo/Makefile
	osinfo/libosinfo-1.0.pc
	data/Makefile
	data/devices/Makefile
	data/hypervisors/Makefile
	data/oses/Makefile
	data/install-scripts/Makefile
	data/schemas/Makefile
	tools/Makefile
	test/Makefile
	docs/Makefile
	docs/reference/Makefile
	examples/Makefile
])
AC_OUTPUT
