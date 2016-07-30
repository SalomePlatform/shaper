#!/bin/bash

while [[ $# > 0 ]]; do
  key="$1"

  case $key in
    debug|Debug)
      BUILD_TYPE=Debug
      shift
      ;;
    lcov|coverage)
      BUILD_TYPE=Debug
      USE_TEST_COVERAGE=ON
      shift
      ;;
    *)
      shift
      ;;
  esac
done

source env.sh

CMAKE_ARGS=""
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=${BUILD_TYPE:-Release}"
CMAKE_ARGS="${CMAKE_ARGS} -DUSE_TEST_COVERAGE=${USE_TEST_COVERAGE:-OFF}"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX:PATH=${SHAPER_ROOT_DIR}"
CMAKE_ARGS="${CMAKE_ARGS} ${SOURCES_DIR}"

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

set -e

cmake -G "Unix Makefiles" ${CMAKE_ARGS}
make -j$(nproc) install
