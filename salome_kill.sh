#!/bin/bash

export ROOT_DIR=$(pwd)/..
export ROOT_DIR=`cd "${ROOT_DIR}";pwd`

source ${ROOT_DIR}/sources/salome_env.sh
source ${ROOT_DIR}/sources/linux_env.sh

INSTALL_DIR=install
if [ $1 ]; then INSTALL_DIR=${INSTALL_DIR}-$1; fi

# Correcting path which defined with error
export LD_LIBRARY_PATH=${KERNEL_ROOT_DIR}/lib/salome:${LD_LIBRARY_PATH}
export SalomeAppConfig=${ROOT_DIR}/${INSTALL_DIR}/share/salome/resources/newgeom:${GUI_ROOT_DIR}/share/salome/resources/gui

${PYTHONBIN} "${KERNEL_ROOT_DIR}/bin/salome/envSalome.py"
${PYTHONBIN} "${KERNEL_ROOT_DIR}/bin/salome/killSalome.py"
