######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.1.0                        #
#            DATE     : 03/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

This directory contain some files to help you to build a package on distributions, please read this file before using it.

I. UBUNTU / DEBIAN :
____________________

This project support the debian packing to build .dev files. To build such package you may use the files in packaging/debian.
First create a directory in your home dir, place the project archive in it and rename it to malt_0.1.0.orig.tar.gz.

To setup your working environneemnt, you will found documentation on 
https://wiki.ubuntu.com/PbuilderHowto or https://wiki.debian.org/IntroDebianPackaging. We mosly need :

--------------------------------------------------------------------
sudo apt-get install pbuilder debootstrap devscripts cdbs debhelper
sudo pbuilder create
--------------------------------------------------------------------

If you work with git version you can build the archive with :

--------------------------------------------------------------------
$user> git archive --prefix=malt-0.1.0/ 0.1.0 | gzip > malt_0.1.0.orig.tar.gz
--------------------------------------------------------------------

Go to your directory and extract malt_0.1.0.orig.tar.gz with and go in the directory.

--------------------------------------------------------------------
$user> tar -xvzf malt-0.1.0.orig.tar.gz
$user> cd malt-0.1.0
--------------------------------------------------------------------

Now go to the malt-0.0.1 directory and copy the packaging/debian into debian :

--------------------------------------------------------------------
$user> cp -r packaging/debian debian
--------------------------------------------------------------------

You can quickly build the package for the running debian with (remove -uc and -us if you have some GPG keys to sign the package) :

--------------------------------------------------------------------
$user> debuild -uc -us
--------------------------------------------------------------------

You may get a .deb archive in the parent directory.

You can build a source package with :

--------------------------------------------------------------------
$user> debuild -S -uc -us
--------------------------------------------------------------------

So you can build package with your source package by running :

--------------------------------------------------------------------
sudo pbuilder build ../*.dsc
--------------------------------------------------------------------

You will obtain the package in /var/cache/pbuilder/results.
