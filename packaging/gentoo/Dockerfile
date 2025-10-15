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
FROM docker.io/gentoo/stage3:latest

#workdir
WORKDIR /workdir

# update list of packages and system
RUN emerge-webrsync && yes | getuto && emerge --verbose --update --deep --newuse @world -k -g
# install deps for build system
RUN emerge -k -g cmake gcc make
# deps for the project
RUN emerge -k -g sys-libs/libunwind dev-libs/elfutils dev-libs/openssl media-gfx/graphviz \
                 dev-cpp/nlohmann_json dev-cpp/cpp-httplib dev-lang/python:3 dev-cpp/gtest \
                 dev-libs/iniparser
# to play with the git sources
RUN yum install -y dev-vcs/git net-libs/nodejs net-misc/curl
