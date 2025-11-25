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
# Because VITE does not support not having node_modules locally
if [[ ${NODE_PATH} != './node_modules' ]]; then
	pwd
	ln -sf ${NODE_PATH} ./node_modules
	rm -f ./node_modules/node_modules
	npm run build
	if [[ -L ./node_modules ]]; then
		rm ./node_modules
	fi
else
	npm run build
fi
