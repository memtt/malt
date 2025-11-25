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
if [[ ${MALT_VITE_BUILD_DIR} != '' && ${MALT_VITE_BUILD_DIR} != '.' ]]; then
	export MALT_VITE_ORIG_DIR="${PWD}"
	mkdir -p "${MALT_VITE_BUILD_DIR}"
	for tmp in .* *.*; do
		cp "${PWD}/$tmp" "${MALT_VITE_BUILD_DIR}/$tmp"
	done
	cp -r "${PWD}/src" "${MALT_VITE_BUILD_DIR}"
	cp -r "${PWD}/public" "${MALT_VITE_BUILD_DIR}"
	cd "${MALT_VITE_BUILD_DIR}"
	npm run build
else
	npm run build
fi
