#!/bin/bash
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 11/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/webview/client-files/prepare.sh
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
export PATH=./node_modules/.bin/:$PATH
SOURCE_DIR=$PWD
BUILD='yes'

############################################################
if [[ "$1" == "--no-build" ]]; then
	shift 1
	BUILD='no'
fi

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
rm -rfd ./../../../extern-deps/node_modules
cd ./../../../extern-deps
ln -sf ../src/webview/client-files/package.json ./package.json
ln -sf ../src/webview/client-files/package-lock.json ./package-lock.json
npm install
cd -
if [[ ${BUILD} == 'yes' ]]; then
	ln -sf ./../../../extern-deps/node_modules ./
	npm run build
	rm -f node_modules
fi
set +x
if [ ! -z "$1" ]; then
	echo > "${1}/deps-loaded"
fi
