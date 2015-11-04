#!/bin/bash

source env.sh
source ${TOOLS_DIR}/env_standalone.sh

CMAKE_ARGS=""
if [ -f GCOV ]; then 
  echo "Use GCOV..."
  source env_lcov.sh
  CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Debug"
  CMAKE_ARGS="${CMAKE_ARGS} -DUSE_TEST_COVERAGE=ON"
elif [ -f DEBUG ]; then
  CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Debug"
else
  CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Release"
fi
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX:PATH=${NEWGEOM_ROOT_DIR}"
CMAKE_ARGS="${CMAKE_ARGS} ${SOURCES_DIR}"

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

set -e

cmake -G "Unix Makefiles" ${CMAKE_ARGS}
make -j$(nproc) install
