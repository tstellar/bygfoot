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

# FIXME: This is a work-around for bygfoot not being able to find the
# bygfoot_constants file when its packaged this way.

mv $installdir/bin/bygfoot $installdir/bin/.bygfoot-bin


cat <<EOF > $installdir/bin/bygfoot
#!/bin/bash
cd \`dirname "\$0"\`
cd ../share/bygfoot
../../bin/.bygfoot-bin
EOF

chmod a+x $installdir/bin/bygfoot

tar -cjf bygfoot-$version.tar.bz2 $installdir

