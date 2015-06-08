#!/bin/bash

# Find absolute path to ROOT_DIR
export ROOT_DIR=$(pwd)/..
export ROOT_DIR=`cd "${ROOT_DIR}";pwd`

SRC_DIR=${ROOT_DIR}/sources

BUILD_DIR=build
INSTALL_DIR=install
if [ $1 ]; then
  BUILD_DIR=${BUILD_DIR}-$1
  INSTALL_DIR=${INSTALL_DIR}-$1
fi

source ${SRC_DIR}/salome_env.sh
source ${SRC_DIR}/linux_env.sh $1
mkdir -p ${ROOT_DIR}/${BUILD_DIR}
cd ${ROOT_DIR}/${BUILD_DIR}

CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Release"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX:PATH=${ROOT_DIR}/${INSTALL_DIR}"
CMAKE_ARGS="${CMAKE_ARGS} ${SRC_DIR}"

ctest 
