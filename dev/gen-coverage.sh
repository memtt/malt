#!/bin/bash
######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 10/2020                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
# This script aimed at making easier to quickly generate
# coverage results

######################################################
set -e
set -x

######################################################
lcov -o out.info -c -d .
lcov -o out.info --remove out.info '/usr/*' '*/Test*' '*/tests/*' '*/extern-deps/*'
genhtml -o html out.info
