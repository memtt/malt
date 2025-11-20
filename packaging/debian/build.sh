#!/bin/bash
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 10/2025
#    LICENSE  : CeCILL-C
#    FILE     : packaging/debian/build.sh
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

# check
if [[ $1 == '' ]]; then
	echo "Usage: $0 {VERSION}" 1>&2
	exit 1
fi

# set bash mode
set -e
set -x

# Get version
VERSION="${1}"

# Extract the sources
tar -xf /sources/malt-${VERSION}.tar.bz2

# move debian dir
cd malt-${VERSION}
cp -r packaging/debian/pkg debian

# build
debuild -uc -us

# Copy out
cp malt-*${VERSION}*.deb* /sources/

# end command print
set +x

# Final
echo "----------------------- DONE -----------------------"
ls -l malt-*${VERSION}*.deb*
