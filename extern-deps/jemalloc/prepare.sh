#!/bin/bash
###########################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 05/2025
#    LICENSE  : CeCILL-C
#    FILE     : prepare.sh
#----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2024
###########################################################

###########################################################
set -e
set -u
set -x

###########################################################
JEMALLOC_VERSION=5.3.0

###########################################################
if [ ! -d "./jemalloc-${JEMALLOC_VERSION}" ]; then
    wget https://github.com/jemalloc/jemalloc/releases/download/${JEMALLOC_VERSION}/jemalloc-${JEMALLOC_VERSION}.tar.bz2
    tar -xvf "./jemalloc-${JEMALLOC_VERSION}.tar.bz2"
    rm -f "./jemalloc-${JEMALLOC_VERSION}.tar.bz2"
fi
