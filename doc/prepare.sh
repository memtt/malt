#!/usr/bin/bash
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 11/2025
#    LICENSE  : CeCILL-C
#    FILE     : doc/prepare.sh
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

set -e
set -u
set -x

VENV_DIR=`mktemp -d`
python3 -m venv ${VENV_DIR}/venv
source ${VENV_DIR}/venv/bin/activate
pip install -r requirements.txt
make html
make latexpdf
mkdir -p dist
mv build/html ./dist
mv build/latex/malt.pdf ./dist
rm -rfvd ./build
rm -rfvd "${VENV_DIR}"
