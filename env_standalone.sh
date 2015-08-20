#!/bin/bash

a_dir=$(dirname $0)

source ${a_dir}/env_linux.sh

export LD_LIBRARY_PATH=${KERNEL_ROOT_DIR}/lib/salome:${LD_LIBRARY_PATH}
export LightAppConfig=${INSTALL_DIR}/share/salome/resources/newgeom:${GUI_ROOT_DIR}/share/salome/resources/gui
