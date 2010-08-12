AC_INIT([libosinfo], [0.2], [virt-tools-list@redhat.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.11.1 -Wall -Werror foreign color-tests])
AC_PREREQ([2.61])
AC_CONFIG_SRCDIR([osinfo/osinfo_common.c])
AC_CONFIG_HEADERS([config.h])
AC_PROG_LIBTOOL
AC_PROG_CC

LIBXML_REQUIRED=2.6.0

#PKG_CHECK_MODULES([GLIB], [glib-2.0], [GLIB_FOUND=yes], [GLIB_FOUND=no])
PKG_CHECK_MODULES([GOBJECT], [gobject-2.0], [GOBJECT_FOUND=yes], [GOBJECT_FOUND=no])

dnl ==========================================================================
dnl find libxml2 library, borrowed from xmlsec
dnl ==========================================================================
LIBXML_CONFIG="xml2-config"
LIBXML_CFLAGS=""
LIBXML_LIBS=""
LIBXML_FOUND="no"

AC_ARG_WITH([libxml], AC_HELP_STRING([--with-libxml=@<:@PFX@:>@],
            [libxml2 location]))
if test "x$with_libxml" = xno ; then
    AC_MSG_CHECKING([for libxml2 libraries >= $LIBXML_REQUIRED])
    AC_MSG_ERROR([libxml2 >= $LIBXML_REQUIRED is required for libvirt])
elif test "x$with_libxml" = "x" -a "x$PKG_CONFIG" != "x" ; then
    PKG_CHECK_MODULES([LIBXML], [libxml-2.0 >= $LIBXML_REQUIRED],
                      [LIBXML_FOUND=yes], [LIBXML_FOUND=no])
fi
if test "$LIBXML_FOUND" = "no" ; then
    if test "x$with_libxml" != "x" ; then
	LIBXML_CONFIG=$with_libxml/bin/$LIBXML_CONFIG
    fi
    AC_MSG_CHECKING(libxml2 $LIBXML_CONFIG >= $LIBXML_REQUIRED )
    if ! $LIBXML_CONFIG --version > /dev/null 2>&1 ; then
	AC_MSG_ERROR([Could not find libxml2 (see config.log for details).])
    fi
    vers=`$LIBXML_CONFIG --version | awk -F. '{ printf "%d", ($1 * 1000 + $2) * 1000 + $3;}'`
    minvers=`echo $LIBXML_REQUIRED | awk -F. '{ printf "%d", ($1 * 1000 + $2) * 1000 + $3;}'`
    if test "$vers" -ge "$minvers" ; then
        LIBXML_LIBS="`$LIBXML_CONFIG --libs`"
        LIBXML_CFLAGS="`$LIBXML_CONFIG --cflags`"
	LIBXML_FOUND="yes"
        AC_MSG_RESULT([yes])
    else
        AC_MSG_ERROR(
          [You need at least libxml2 $LIBXML_REQUIRED for this version of libvirt])
    fi
fi
AC_SUBST([LIBXML_CFLAGS])
AC_SUBST([LIBXML_LIBS])

AC_CONFIG_FILES([
	Makefile
	osinfo/Makefile
	test/Makefile
])
AC_OUTPUT
