#!/bin/bash -x

export SALOME_DIR=/dn46/SALOME/series9x/current-2019-03-19

# Path to sources
export SOURCES_DIR=$(pwd)
# Path to build directory
export BUILD_DIR=$(cd ${SOURCES_DIR}/.. && pwd)/build
# Path to linux_run.sh & salome_run.sh
export AUT_DIR=$(pwd)

# Path to env_*.sh files
export TOOLS_DIR=$(pwd)

source ${SALOME_DIR}/start.sh

# Path to special products dir (lcov, cpplint)
export SHAPER_PDIR=/dn48/newgeom/common/products

# Path to install directory
export SHAPER_ROOT_DIR=$(cd ${SOURCES_DIR}/.. && pwd)/install

source ${TOOLS_DIR}/env_linux.sh
source ${TOOLS_DIR}/env_salome.sh
