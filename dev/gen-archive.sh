#!/bin/bash
######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.2.0                        #
#            DATE     : 04/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
#extract version
version=0.2.0
prefix=malt-${version}

######################################################
echo "Generate ${prefix}.tar.gz..."
git archive --format=tar --prefix=${prefix}/ HEAD | gzip > ${prefix}.tar.gz
echo "Finished"
