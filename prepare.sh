#!/bin/bash
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 05/2025
#    LICENSE  : CeCILL-C
#    FILE     : prepare.sh
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

###########################################################
set -e
set -u
set -x

###########################################################
# prepare webview
cd ./src/webview
./prepare.sh
cd -

###########################################################
# prepare webview client (download & pre-build)
cd ./src/webview/client-files
./prepare.sh
cd -

###########################################################
cd ./extern-deps/jemalloc/
./prepare.sh
cd -
