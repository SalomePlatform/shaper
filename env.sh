#!/bin/bash -x

set -o nounset

if [[ ! -f env_config.sh ]]; then
  echo "Can not find env_config.sh. Please, run config.sh."
  exit 1
fi
source env_config.sh

export ROOT_DIR=$(cd $(dirname $0)/.. && pwd)
export SOURCES_DIR=${ROOT_DIR}/sources
export BUILD_DIR=${ROOT_DIR}/build${PLATFORM}
export INSTALL_DIR=${ROOT_DIR}/install${PLATFORM}

export NEWGEOM_PDIR=/dn48/newgeom/common/products
