#!/bin/bash
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 12/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/webview/prepare.sh
#-----------------------------------------------------------
#    AUTHOR   : Antoine Bernard (crans.org) - 2024
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
export PATH=./node_modules/.bin/:$PATH
SOURCE_DIR=$PWD

############################################################
if [ ! -z "$1" ]
then
	cd "$1"
fi

############################################################
if ! which npm
then
	echo "Nodejs 'npm' command required to fetch the webview client dependencies, please install 'nodejs' before proceeding or download a release archive instead of using master branch"
	exit 1
fi

############################################################
#Workaround on debian:jessie and ubuntu:16.X which rename
#node executable as nodejs which break bower
if ! which node
then
	echo "You should install NodeJS to fetch web GUI components"
	echo "Or download a release archive as it already contains all those files"
	exit 1
fi

############################################################
set -e
set -x
ln -sf ../../../extern-deps/node_modules ./
npm install --pefix ../../../extern-deps/node_modules
set +x
if [ ! -z "$1" ]; then
	echo > "${1}/deps-loaded"
fi
