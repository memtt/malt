#!/bin/bash

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

# Put the sources in place
mkdir -p ~/rpmbuild/SOURCES/
cp -f /sources/malt-${VERSION}.tar.bz2 ~/rpmbuild/SOURCES/

# Get the .spec & build
tar -xf ~/rpmbuild/SOURCES/malt-${VERSION}.tar.bz2 malt-${VERSION}/packaging/fedora/malt.spec
rpmbuild -ba malt-${VERSION}/packaging/fedora/malt.spec

# Copy out
cp ~/rpmbuild/?RPMS/*/malt-*${VERSION}*.rpm /sources/

# end command print
set +x

# Final
echo "----------------------- DONE -----------------------"
ls -l malt-*${VERSION}*.rpm
