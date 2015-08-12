#!/bin/bash -x

set -o nounset

PLATFORM=""
if [ -f PLATFORM ]; then
PLATFORM="$(lsb_release -is)"
RELEASE="$(lsb_release -rs)"
if [ "${PLATFORM}" = 'CentOS' ]; then PLATFORM=-centos
elif [ "${PLATFORM}" = 'Debian' ]; then
  if [ "${RELEASE}" = '8.0' ]; then PLATFORM=-d8
  else PLATFORM=-d6
  fi
fi
fi

export ROOT_DIR=$(cd $(dirname $0)/.. && pwd)
export SOURCES_DIR=${ROOT_DIR}/sources
export BUILD_DIR=${ROOT_DIR}/build${PLATFORM}
export INSTALL_DIR=${ROOT_DIR}/install${PLATFORM}

export NEWGEOM_PDIR=/dn48/newgeom/common/products
