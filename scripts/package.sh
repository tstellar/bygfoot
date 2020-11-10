#!/bin/bash

set -ex

srcdir=$1

version="2.3.3"

if [ -n "$BYGFOOT_VERSION" ]; then
    version=$BYGFOOT_VERSION
fi

builddir=./build-$version
installdir=./bygfoot-$version

mkdir -p $builddir
mkdir -p $installdir

cmake -G Ninja  -B $builddir -S $srcdir -DCMAKE_INSTALL_PREFIX=$installdir -DCMAKE_C_FLAGS=-DVERS=\"\\\"$version\\\"\"
ninja -v -C $builddir install

# Fixup install dir so that bygfoot can find the support
# files.

mv $installdir/bin/bygfoot $installdir/
rmdir $installdir/bin

mv $installdir/share/bygfoot/support_files $installdir

mv $installdir/share/locale $installdir
rmdir $installdir/share/bygfoot
rmdir $installdir/share

# The original package script did this, so lets do it too.
for f in AUTHORS COPYING ChangeLog INSTALL README TODO UPDATE ReleaseNotes; do
    cp $srcdir/$f $installdir/
done

tar -cjf bygfoot-$version.tar.bz2 $installdir

