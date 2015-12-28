#!/bin/bash

source env.sh

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
    standalone|Standalone)
      MODE=Standalone
      shift
      ;;
    *)
      shift
      ;;
  esac
done

if [[ ${MODE} = 'Standalone' ]]; then
  source ${TOOLS_DIR}/env_standalone.sh
else
  source ${TOOLS_DIR}/env_salome.sh
fi

CMAKE_ARGS=""
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=${BUILD_TYPE:-Release}"
CMAKE_ARGS="${CMAKE_ARGS} -DUSE_TEST_COVERAGE=${USE_TEST_COVERAGE:-OFF}"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX:PATH=${NEWGEOM_ROOT_DIR}"
CMAKE_ARGS="${CMAKE_ARGS} ${SOURCES_DIR}"

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

set -e

cmake -G "Unix Makefiles" ${CMAKE_ARGS}
make -j$(nproc) install
