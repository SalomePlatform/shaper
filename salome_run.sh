#!/bin/sh

export ROOT_DIR=$(pwd)/..
export ROOT_DIR=`cd "${ROOT_DIR}";pwd`

source ${ROOT_DIR}/sources/linux_env.sh

# Correcting path which defined with error
export LD_LIBRARY_PATH=${KERNEL_ROOT_DIR}/lib/salome:${LD_LIBRARY_PATH}
export SalomeAppConfig=${ROOT_DIR}/install/share/salome/resources/newgeom:${GUI_ROOT_DIR}/share/salome/resources/gui

${PYTHONBIN} "${KERNEL_ROOT_DIR}/bin/salome/envSalome.py"
${PYTHONBIN} "${KERNEL_ROOT_DIR}/bin/salome/runSalome.py"
