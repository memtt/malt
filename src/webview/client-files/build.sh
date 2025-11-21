#!/bin/bash

###############################################################################
set -e
set -u
set -x

###############################################################################
export MALT_VITE_BUILD_DIR="."
if [[ ${1} != "" ]]; then
	export MALT_VITE_BUILD_DIR="${1}"
fi
if [[ ${2} != "" ]]; then
	export NODE_PATH="${2}"
	export PATH="${NODE_PATH}/.bin:$PATH"
fi

###############################################################################
ln -sf ${NODE_PATH} ./node_modules
npm run build
rm ./node_modules
