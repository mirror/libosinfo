#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

(test -f $srcdir/osinfo/osinfo_db.c) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level libosinfo directory"
    exit 1
}

which gnome-autogen.sh || {
    echo "You need to install gnome-common from the GNOME git"
    exit 1
}

# Real ChangeLog/AUTHORS is auto-generated from GIT logs at
# make dist time, but automake requires that it
# exists at all times :-(
touch ChangeLog AUTHORS

ACLOCAL_FLAGS="$ACLOCAL_FLAGS" . gnome-autogen.sh --enable-gtk-doc "$@"
