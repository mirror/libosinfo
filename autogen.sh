#!/bin/sh
libtoolize
aclocal
autoheader
autoconf --force
automake --add-missing
./configure
