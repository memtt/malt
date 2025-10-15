############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 07/2025
#    LICENSE  : CeCILL-C
#    FILE     : packaging/Dockerfile-debian
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2019 - 2024
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

#distri we want
FROM debian:trixie

#workdir
WORKDIR /workdir

# upgrade
RUN pacman --noconfirm -Syu
# install deps for build system
RUN pacman --noconfirm -Sy cmake gcc make
# deps for the project
#  - Missing on this distro : cpp-httplib
RUN pacman --noconfirm -Sy libunwind libelf openssl graphviz nlohmann-json python3 gtest iniparser
# to play with the git sources
RUN pacman --noconfirm -Sy git nodejs npm curl
