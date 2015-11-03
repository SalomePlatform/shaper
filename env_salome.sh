#!/bin/bash

source ${TOOLS_DIR}/env_linux.sh

# Correcting path which defined with error
export LD_LIBRARY_PATH=${KERNEL_ROOT_DIR}/lib/salome:${LD_LIBRARY_PATH}
export SalomeAppConfig=${NEWGEOM_ROOT_DIR}/share/salome/resources/newgeom:${GUI_ROOT_DIR}/share/salome/resources/gui
