#!/bin/bash
######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.0.0                        #
#            DATE     : 02/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
#This script remove the special chars from C strings test report
filename="$1"
tmp="$(cat $filename)"
tmp="$(echo -e "$tmp" | sed -e 's/\\\"/"/g')"
echo "$tmp" >  "$filename"
