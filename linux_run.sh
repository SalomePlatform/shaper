#!/bin/bash

# Find absolute path to ROOT_DIR
export ROOT_DIR=$(pwd)/..
export ROOT_DIR=`cd "${ROOT_DIR}";pwd`

SRC_DIR=${ROOT_DIR}/sources
source ${SRC_DIR}/salome_env.sh
source ${SRC_DIR}/linux_env.sh $1

INSTALL_DIR=install
if [ $1 ]; then INSTALL_DIR=${INSTALL_DIR}-$1; fi

${ROOT_DIR}/${INSTALL_DIR}/bin/GeomApp
