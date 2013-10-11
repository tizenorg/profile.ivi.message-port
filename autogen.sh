#!/bin/sh -e

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

mkdir -p m4
aclocal #-I m4 
autoheader 
libtoolize --force --copy
autoconf 
automake --add-missing --copy
autoreconf --install --force
. $srcdir/configure "$@"

