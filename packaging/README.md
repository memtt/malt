Packaging
=========

This directory contain some files to help you to build a package on distributions, please read this file before using it.

For quicker usage you can use the script in dev directory for root directory :

	./dev/packaging.sh {debian|debian-pbuilder|fedora|rpm}

I. UBUNTU / DEBIAN :
--------------------

This project support the debian packing to build .dev files. To build such package you may use the files in packaging/debian.
First create a directory in your home dir, place the project archive in it and rename it to _1.2.2.orig.tar.gz.

To setup your working environneemnt, you will found documentation on 
https://wiki.ubuntu.com/PbuilderHowto or https://wiki.debian.org/IntroDebianPackaging. We mosly need :

	sudo apt-get install pbuilder debootstrap devscripts cdbs debhelper
	sudo pbuilder create

If you work with git version you can build the archive with :

	git archive --prefix=malt-1.2.2/ v1.2.2 | gzip > malt_1.2.2.orig.tar.gz

Go to your directory and extract _1.2.2.orig.tar.gz with and go in the directory.

	tar -xvzf malt_1.2.2.orig.tar.gz
	cd malt-1.2.2

Now go to the -0.0.1 directory and copy the packaging/debian into debian :

	cp -r packaging/debian debian

You can quickly build the package for the running debian with (remove -uc and -us if you have some GPG keys to sign the package) :

	debuild -uc -us

You may get a .deb archive in the parent directory.

You can build a source package with :

	debuild -S -uc -us

So you can build package with your source package by running :

	sudo pbuilder build ../*.dsc

You will obtain the package in /var/cache/pbuilder/results.



II. FEDORA / CENTOS / REDHAT :
------------------------------

You can find more documentation on http://fedoraproject.org/wiki/How_to_create_an_RPM_package.
First ensure to get the required tools :

	sudo yum install rpmdevtools yum-utils cmake 
	rpmdev-setuptree

Now generate the archive file (or download it from internet) :

	git archive --prefix=-1.2.2/ 1.2.2 | bzip2 > ~/rpmbuild/SOURCES/-1.2.2.tar.bz2

And build the package :

	rpmbuild -ba packaging/fedora/malt.spec

Now you may get *.rpm files in ~/rpmbuild/RPMS and ~/rpmbuild/SRPMS dirs.

III. GENTOO :
-------------

This project support a Gentoo integration, for that you may found the corresponding ebuild file int the packaging/gentoo 
subdirectory. To use this, simply place the dev-libs/malt directory on your own portage overlay.

For overlay documentation see : http://www.gentoo.org/proj/en/overlays/userguide.xml.

--------------------------------------------------------------------
#root> mkdir /usr/local/portage/myoverlay
#root> cp -r dev-libs /usr/local/portage/myoverlay/
--------------------------------------------------------------------

Add "/usr/local/portage/myoverlay" in your PORTDIR_OVERLAY variable in /etc/make.conf.

Now get the project archive and place it in /usr/portage/distfiles (or corresponding dir if you change it in make.conf). If 
you work on the git version, you can build the project archive with :

--------------------------------------------------------------------
$user> git archive --prefix=malt-1.2.2/ v1.2.2 | gzip > malt-1.2.2.tar.gz
--------------------------------------------------------------------

Now got to the /usr/local/portage/myoverlay/dev-libs and build the Manifest file :

--------------------------------------------------------------------
#root> ebuild /usr/local/portage/myoverlay/dev-libs/malt-1.2.2.ebuild manifest
--------------------------------------------------------------------

Ok now you can test the build septs one by one :

--------------------------------------------------------------------
#root> ebuild /usr/local/portage/myoverlay/dev-libs/malt-1.2.2.ebuild clean
#root> ebuild /usr/local/portage/myoverlay/dev-libs/malt-1.2.2.ebuild fetch
#root> ebuild /usr/local/portage/myoverlay/dev-libs/malt-1.2.2.ebuild unpack
#root> ebuild /usr/local/portage/myoverlay/dev-libs/malt-1.2.2.ebuild prepare
#root> ebuild /usr/local/portage/myoverlay/dev-libs/malt-1.2.2.ebuild configure
#root> ebuild /usr/local/portage/myoverlay/dev-libs/malt-1.2.2.ebuild compile
#root> ebuild /usr/local/portage/myoverlay/dev-libs/malt-1.2.2.ebuild test
--------------------------------------------------------------------

... (I don't do install here bu you also can do preinst, install, postinst, qmerge (maybe qmerge is not a goot idea for the 
fist testings steps).

Or you can simply run :

--------------------------------------------------------------------
#root> echo "=dev-libs/malt-1.2.2" >> /etc/portage/package.keywords
#root> emerge -a malt
--------------------------------------------------------------------
