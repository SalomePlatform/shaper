#!/bin/bash -x

# Path to sources
export SOURCES_DIR=$(pwd)
# Path to build directory
export BUILD_DIR=$(cd ../build && pwd)
# Path to linux_run.sh & salome_run.sh
export AUT_DIR=$(pwd)

# Path to env_*.sh files
export TOOLS_DIR=$(pwd)

# Extracts INST_ROOT directory for SALOME
if [[ ! -f env_config.sh ]]; then
  echo "Can not find env_config.sh. Please, run config.sh."
  exit 1
fi
source env_config.sh

# Path to solvespace-2
export NEWGEOM_PDIR=/dn48/newgeom/common/products
# Path to install directory
export NEWGEOM_ROOT_DIR=$(cd ../install && pwd)
