#!/bin/bash

source env.sh
source env_standalone.sh

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

CMAKE_ARGS=""
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Release"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}"
CMAKE_ARGS="${CMAKE_ARGS} ${SOURCES_DIR}"

cmake -G "Unix Makefiles" ${CMAKE_ARGS}
make -j$(nproc) install
