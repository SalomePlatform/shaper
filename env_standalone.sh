#!/bin/bash

pushd ${SOURCES_DIR}
  source env_linux.sh
popd

export LD_LIBRARY_PATH=${KERNEL_ROOT_DIR}/lib/salome:${LD_LIBRARY_PATH}
export LightAppConfig=${INSTALL_DIR}/share/salome/resources/newgeom:${GUI_ROOT_DIR}/share/salome/resources/gui
