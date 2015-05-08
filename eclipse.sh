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

export LD_LIBRARY_PATH=${KERNEL_ROOT_DIR}/lib/salome:${LD_LIBRARY_PATH}
export LightAppConfig=${ROOT_DIR}/${INSTALL_DIR}/share/salome/resources/newgeom:${GUI_ROOT_DIR}/share/salome/resources/gui

CMAKE_ARGS="-D_ECLIPSE_VERSION=4.3"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Debug"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=ON"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX:PATH=${ROOT_DIR}/${INSTALL_DIR}"
CMAKE_ARGS="${CMAKE_ARGS} -DPYTHON_EXECUTABLE=${PYTHONHOME}/bin/python"
CMAKE_ARGS="${CMAKE_ARGS} -DUSE_TEST_COVERAGE=OFF"
CMAKE_ARGS="${CMAKE_ARGS} ${SRC_DIR}"

cmake -G "Eclipse CDT4 - Unix Makefiles" ${CMAKE_ARGS}

#/misc/dn48/newgeom/common/eclipse-4.4.0/eclipse&
