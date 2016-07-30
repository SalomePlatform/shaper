#!/bin/bash

# Correcting path which defined with error
export LD_LIBRARY_PATH=${KERNEL_ROOT_DIR}/lib/salome:${LD_LIBRARY_PATH}
export SalomeAppConfig=${SHAPER_ROOT_DIR}/share/salome/resources/shaper:${GUI_ROOT_DIR}/share/salome/resources/gui
