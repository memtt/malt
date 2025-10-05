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
RUN apt update && apt upgrade -y
# install deps for build system
RUN apt install -y build-essential make gcc cmake g++
# deps for the project
RUN apt install -y libunwind-dev libelf-dev libssl-dev graphviz nlohmann-json3-dev libcpp-httplib-dev python3-dev googletest
# to play with the git sources
RUN apt install -y git nodejs curl
# to build the RPM
RUN apt install -y pbuilder debootstrap devscripts cdbs debhelper
