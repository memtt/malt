Change log
==========

master
------

 - Add scatter plots (lifetime,size) and (size,time).
 - Add maltEnable/maltDisable to limit to sub-part of a program.
 - Profile file is not anymore indented by default
 - Add options to reduce size of output file (stackTree and loopSuppress). Not enabled by default.
 - Add DebugDummy to avoid unneeded allocation in Release mode
 - Add help with MPI in README
 - Add manpages for malt, malt-passwd and malt-webserver
 - Add packaging files for gentoo & archlinux
 - Use a more compress stack tree in json file
 - Add filter to only instrument the given executable and disabling childs
 - Add warnings about global varibales if the app use too many of them

0.3.0 - 16/06/2015
------------------

 - Add http authentication to secure the web GUI.
 - Add silent option (-q/--quiet)
 - Add password authentication inside http protocol to secure the web GUI
 - Add support for packaging (Fedora/Centos/Debian/Ubuntu...)
 - Fix error in unit tests
 - Fix bug in ELFReader

0.2.0 - 29/04/2015
------------------

 - Add scatter chart on ralloc page.
 - Get grunt script to generate shorter standalone version with compressed JS/CSS
 - Add support for data export and svg export on some charts
 - Bugfix: add missing modules in package.js
 - Bugfix: index times on time charts

0.1.0 - 26/03/2015
------------------

 - First tagged release don't list all new things.