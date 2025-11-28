############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 07/2025
#    LICENSE  : CeCILL-C
#    FILE     : packaging/Dockerfile-fedora
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2019
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

#distri we want
FROM fedora:41

#workdir
WORKDIR /workdir

# update list of packages and system
RUN dnf update -y && dnf upgrade -y
# install deps for build system
RUN yum install -y make gcc gcc-c++ cmake
# deps for the project
RUN yum install -y libunwind-devel elfutils-devel graphviz nlohmann-json-devel cpp-httplib-devel python3-devel gtest libiniparser-devel
# to play with the git sources
RUN yum install -y git nodejs curl
# to build the RPM
RUN yum install -y rpm-build rpmdevtools
