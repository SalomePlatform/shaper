#!/bin/bash

source env.sh
source env_standalone.sh

CMAKE_ARGS=""
if [ -f GCOV ]; then 
  echo "Use GCOV..."
  source env_lcov.sh
  CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Debug"
  CMAKE_ARGS="${CMAKE_ARGS} -DUSE_TEST_COVERAGE=ON"
else
  CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Release"
fi
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}"
CMAKE_ARGS="${CMAKE_ARGS} ${SOURCES_DIR}"

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

cmake -G "Unix Makefiles" ${CMAKE_ARGS}
make -j$(nproc) install
