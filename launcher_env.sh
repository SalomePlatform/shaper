#!/bin/bash

# Find absolute path to ROOT_DIR
export ROOT_DIR=$(pwd)/..
export ROOT_DIR=`cd "${ROOT_DIR}";pwd`

PLATFORM="$(lsb_release -is)"
RELEASE="$(lsb_release -rs)"
if [ "${PLATFORM}" == "CentOS" ]; then PLATFORM=centos
elif [ "${PLATFORM}" == "Debian" ]; then 
  if [ "${RELEASE}" == "8.0" ]; then PLATFORM=d8
  else PLATFORM=d6
  fi
fi

SRC_DIR=${ROOT_DIR}/sources
source ${SRC_DIR}/salome_env.sh
source ${SRC_DIR}/linux_env.sh ${PLATFORM}

BUILD_DIR=build
INSTALL_DIR=install
if [ ${PLATFORM} ]; then
  BUILD_DIR=${BUILD_DIR}-${PLATFORM}
  INSTALL_DIR=${INSTALL_DIR}-${PLATFORM}
fi

mkdir -p ${ROOT_DIR}/${BUILD_DIR}
cd ${ROOT_DIR}/${BUILD_DIR}

export LD_LIBRARY_PATH=${KERNEL_ROOT_DIR}/lib/salome:${LD_LIBRARY_PATH}
export LightAppConfig=${ROOT_DIR}/${INSTALL_DIR}/share/salome/resources/newgeom:${GUI_ROOT_DIR}/share/salome/resources/gui

if [ $@ ]; then
  $@
else
  bash
fi
