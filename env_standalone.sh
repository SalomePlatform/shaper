#!/bin/bash

for path in TOOLS_DIR; do
  if [[ -z "${!path}" ]]; then 
    echo "${path} not found."; exit 1
  else
    echo "Found ${path}: ${!path}"
  fi
done

source ${TOOLS_DIR}/env_linux.sh

export LD_LIBRARY_PATH=${KERNEL_ROOT_DIR}/lib/salome:${LD_LIBRARY_PATH}
export LightAppConfig=${NEWGEOM_ROOT_DIR}/share/salome/resources/newgeom:${GUI_ROOT_DIR}/share/salome/resources/gui
