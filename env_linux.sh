#!/bin/bash

# This script uses:
#
# SHAPER_PDIR - path of PRODUCTS for SHAPER (lcov, cpplint)
# SHAPER_ROOT_DIR - path of SHAPER installation

for path in SHAPER_PDIR SHAPER_ROOT_DIR; do
  if [[ -z ${!path+x} ]]; then
    echo "${path} not found."; exit 1
  else
    echo "Found ${path}: ${!path}"
  fi
done

#------ lcov ------
export LCOV_ROOT_DIR=${SHAPER_PDIR}/lcov-1.11
export PATH=${LCOV_ROOT_DIR}/bin:${PATH}
##

#------ cpplint ------
export CPPLINT_ROOT_DIR=${SHAPER_PDIR}/cpplint-1.3.0
export PATH=${CPPLINT_ROOT_DIR}:${PATH}
##

#------ SHAPER ------
export PATH=${SHAPER_ROOT_DIR}/bin/salome:${PATH}
export PYTHONPATH=${SHAPER_ROOT_DIR}/bin/salome:${SHAPER_ROOT_DIR}/lib/python3.6/site-packages/salome:${PYTHONPATH}
export LD_LIBRARY_PATH=${SHAPER_ROOT_DIR}/lib/salome:${LD_LIBRARY_PATH}
