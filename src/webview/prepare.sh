#!/bin/bash
######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.3                        #
#            DATE     : 10/2024                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
export PATH=./node_modules/.bin/:$PATH
SOURCE_DIR=$PWD

######################################################
if [ ! -z "$1" ]
then
	cp bower.json "$1"
	cd "$1"
fi

######################################################
if ! which npm
then
	echo "Nodejs 'npm' command required to fetch the webview client dependencies, please install 'nodejs' before proceeding or download a release archive instead of using master branch"
	exit 1
fi

######################################################
#Workaround on debian:jessie and ubuntu:16.X which rename
#node executable as nodejs which break bower
if ! which node
then
	if which nodejs
	then
		echo "Create NodeJS -> Node fix for ubuntu/debian !"
		mkdir -p ./node_modules/.bin/
		ln -s $(which nodejs) ./node_modules/.bin/node
	else
		echo "You should install NodeJS to fetch web GUI components"
		echo "Or download a release archive as it already contains all those files"
		exit 1
	fi
fi

######################################################
set -e
set -x
npm ci
rm -f ./node_modules/.bin/node
set +x
if [ ! -z "$1" ]; then
	echo > deps-loaded
fi

