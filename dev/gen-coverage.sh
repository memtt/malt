#!/bin/bash
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.2
#    DATE     : 09/2024
#    LICENSE  : CeCILL-C
#    FILE     : dev/gen-coverage.sh
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2022 - 2024
############################################################

############################################################
# This script aimed at making easier to quickly generate
# coverage results

############################################################
set -e
set -x

############################################################
lcov -o out.info -c -d .
lcov -o out.info --remove out.info '/usr/*' '*/Test*' '*/tests/*' '*/extern-deps/*'
genhtml -o html out.info
