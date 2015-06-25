Packaging
=========

This directory contain some files to help you to build a package on distributions, please read this file before using it.

For quicker usage you can use the script in dev directory for root directory :

	./dev/packaging.sh {debian|debian-pbuilder|fedora|rpm}

I. UBUNTU / DEBIAN :
--------------------

This project support the debian packing to build .dev files. To build such package you may use the files in packaging/debian.
First create a directory in your home dir, place the project archive in it and rename it to malt_0.2.0.orig.tar.gz.

To setup your working environneemnt, you will found documentation on 
https://wiki.ubuntu.com/PbuilderHowto or https://wiki.debian.org/IntroDebianPackaging. We mosly need :

	sudo apt-get install pbuilder debootstrap devscripts cdbs debhelper
	sudo pbuilder create

If you work with git version you can build the archive with :

	git archive --prefix=malt-0.2.0/ 0.2.0 | gzip > malt_0.2.0.orig.tar.gz

Go to your directory and extract malt_0.2.0.orig.tar.gz with and go in the directory.

	tar -xvzf malt-0.2.0.orig.tar.gz
	cd malt-0.2.0

Now go to the malt-0.0.1 directory and copy the packaging/debian into debian :

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

	git archive --prefix=malt-0.2.0/ 0.2.0 | bzip2 > ~/rpmbuild/SOURCES/malt-0.2.0.tar.bz2

And build the package :

	rpmbuild -ba packaging/fedora/svUnitTest.spec

Now you may get *.rpm files in ~/rpmbuild/RPMS and ~/rpmbuild/SRPMS dirs.
