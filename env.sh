#!/bin/bash -x

# Path to sources
export SOURCES_DIR=$(pwd)
# Path to build directory
export BUILD_DIR=$(cd ${SOURCES_DIR}/.. && pwd)/build
# Path to linux_run.sh & salome_run.sh
export AUT_DIR=$(pwd)

# Path to env_*.sh files
export TOOLS_DIR=$(pwd)

export HOME_OLD=$HOME
# to use patched OCCT 7.1.0 for SHAPER
export CUSTOM_CAS_ROOT=1
export ENV_FOR_LAUNCH=1

source /dn64/series8x/work/init.sh 2016-12-26
export HOME=$HOME_OLD

# Path to solvespace-2
export SHAPER_PDIR=/dn48/newgeom/common/products
# Path to install directory
export SHAPER_ROOT_DIR=$(cd ${SOURCES_DIR}/.. && pwd)/install

source ${TOOLS_DIR}/env_linux.sh
source ${TOOLS_DIR}/env_salome.sh
