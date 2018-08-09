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
    models|test-models)
      ADD_MODELS_TESTS=TRUE
      shift
      ;;
    *)
      shift
      ;;
  esac
done

## Preserve PTHONPATH in order to avoid problems with Sphinx on Fedora26 because of Pigments in ParaView
export PYTHONPATH_OLD=$PYTHONPATH
source env.sh

if [[ ${OS_NAME} = Fedora26-64 ]]
then
  export PYTHONPATH=$PYTHONPATH_OLD
fi

CMAKE_ARGS=""
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=${BUILD_TYPE:-Release}"
CMAKE_ARGS="${CMAKE_ARGS} -DUSE_TEST_COVERAGE=${USE_TEST_COVERAGE:-OFF}"
CMAKE_ARGS="${CMAKE_ARGS} -DADD_MODELS_TESTS=${ADD_MODELS_TESTS:-FALSE}"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX:PATH=${SHAPER_ROOT_DIR}"
CMAKE_ARGS="${CMAKE_ARGS} ${SOURCES_DIR}"

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

set -e

cmake -G "Unix Makefiles" ${CMAKE_ARGS}
make -j$(nproc) install
