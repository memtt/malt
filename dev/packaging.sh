#!/bin/bash
######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0                        #
#            DATE     : 03/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

#setup version
VERSION=0.1.0

#exit on error
set -e
set -x

#Generate debian common package
function debian_common()
{
	#gen archive
	git archive --prefix=malt-${VERSION}/ ${VERSION} | gzip > malt_${VERSION}.orig.tar.gz
	#extract archive
	tar -xzf malt_${VERSION}.orig.tar.gz
	#go to directory
	cd malt-${VERSION}
	#copy debian directory
	cp -r ../packaging/debian debian
}

#generate debian package from current install
function debian_packaging()
{
	#gen and extract archive
	debian_common
	#call debuild
	debuild -uc -us
}

#generate with pbuilder
function debian_pbuilder_packaging()
{
	#gen and extract archive
	debian_common
	#call debuild
	debuild -S -uc -us
	#call pbuilder
	sudo pbuilder build ../malt_${VERSION}.dsc
	#copy
	cp /var/cache/pbuilder/result/malt_${VERSION}_*.deb ../
}

#help
function show_help()
{
	echo "Invalid argument, required build type as parameter" 1>&2
	echo "   $0 {debian|debian-pbuilder}" 1>&2
	exit 1
}


#select mode
case $1 in
	'debian')
		debian_packaging
		;;
	'debian-pbuilder')
		debian_pbuilder_packaging
		;;
	''|'help'|'list')
		show_help
		;;
esac
