#!/bin/sh

export ROOT_DIR=$(pwd)/..

source ${ROOT_DIR}/sources/salome_env.sh

# Correcting path which defined with error
export LD_LIBRARY_PATH=${KERNEL_ROOT_DIR}/lib/salome:${LD_LIBRARY_PATH}

export LightAppConfig=${ROOT_DIR}/install/share/salome/resources/newgeom:${GUI_ROOT_DIR}/share/salome/resources/gui

SUITApp LightApp -style salome --modules=NewGeom --uselicense --noexcepthandling