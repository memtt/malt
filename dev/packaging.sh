#!/bin/bash
######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 10/2020                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

#setup version
VERSION=1.2.2
VERSION=$(echo $VERSION | cut -f 1 -d '-')

#exit on error
set -e
set -x

#Generate debian common package
function debian_common()
{
	#gen archive
	git archive --prefix=malt-${VERSION}/ HEAD | gzip > malt_${VERSION}.orig.tar.gz
	#extract archive
	tar -xzf malt_${VERSION}.orig.tar.gz
	#go to directory
	cd malt-${VERSION}
	#copy debian directory
	cp -r packaging/debian debian
	#build manpages
	update_manpages
}

#generate debian package from current install
function debian_packaging()
{
	#gen and extract archive
	debian_common
	#call debuild
	debuild -uc -us || echo "deb-build has errors, continue"
}

function debian_build_docker()
{
	tag=${1}
	cp packaging/Dockerfile-debian ./ Dockerfile
	docker build --tag ${tag} .
}

#generate debian via docker
function debian_docker_packaging()
{
	#args
	local distro=${1}

	#gen tag
	local tag=malt-build-${distro/:/-}

	#make
	docker image ls | grep -q ${tag} || debian_build_docker ${tag}
	docker run -v $PWD:/workdir ${tag} ./dev/packaging.sh debian
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
	echo "   $0 {debian|debian-pbuilder|fedora|rpm}" 1>&2
	exit 1
}

#rpm
function fedora_packaging()
{
	#rpm tree
	rpmdev-setuptree

	#gen archive
	git archive --prefix=malt-${VERSION}/ HEAD | bzip2 > ./malt-${VERSION}.tar.bz2
	#extract archive
	tar -xzf malt-${VERSION}.tar.bz2
	#go to directory
	cd malt-${VERSION}
	#build manpages
	update_manpages
	#gen archive
	tar -cvjf ~/rpmbuild/SOURCES/malt-${VERSION}.tar.bz2 ./malt-${VERSION}
	#rm
	rm -rfd ./malt-${VERSION}
	#build
	rpmbuild -ba packaging/fedora/malt.spec
	#move here
	mv ~/rpmbuild/SRPMS/malt-${VERSION}*.src.rpm .
	mv ~/rpmbuild/RPMS/*/malt-*.rpm .
}

function fedora_build_docker()
{
	tag=${1}
	cp packaging/Dockerfile-fedora ./ Dockerfile
	docker build --tag ${tag} .
}

function fedora_docker_packaging()
{
	#args
	local distro=${1}

	#gen tag
	local tag=malt-build-${distro/:/-}

	#make
	docker image ls | grep -q ${tag} || fedora_build_docker ${tag} .
	docker run -v $PWD:/workdir ${tag} ./dev/packaging.sh fedora
}

update_manpages()
{
	make -C ./src/manpages || echo "WARNING: Manpages not generated probably because you do not have Ronn installed !"
}

#select mode
case $1 in
	'debian')
		debian_packaging
		;;
	'debian-docker')
		debian_docker_packaging "debian:jessie"
		;;
	'debian-pbuilder')
		debian_pbuilder_packaging
		;;
	'fedora'|'rpm')
		fedora_packaging
		;;
	'fedora-docker'|'rpm-docker')
		fedora_docker_packaging "fedora:30"
		;;
	''|'help'|'list')
		show_help
		;;
esac
