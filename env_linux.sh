#!/bin/bash

# This script uses:
#
# INST_ROOT - path of SALOME (env_products.sh)
# NEWGEOM_PDIR - path of PRODUCTS for NEWGEOM (SolveSpace, lcov)
# NEWGEOM_ROOT_DIR - path of NEWGEOM installation

for path in INST_ROOT NEWGEOM_PDIR NEWGEOM_ROOT_DIR; do
  if [[ -z ${!path+x} ]]; then
    echo "${path} not found."; exit 1
  else
    echo "Found ${path}: ${!path}"
  fi
done

set +u
echo "Source ${INST_ROOT}/$(cd ${INST_ROOT} && ls *env_products.sh)"
source ${INST_ROOT}/$(cd ${INST_ROOT} && ls *env_products.sh)
set -u

#------ SolveSpace ------
export SOLVESPACE_ROOT_DIR=${NEWGEOM_PDIR}/solvespace-2.1
export LD_LIBRARY_PATH=${SOLVESPACE_ROOT_DIR}/lib:${LD_LIBRARY_PATH}
##

#------ lcov ------
export LCOV_ROOT_DIR=${NEWGEOM_PDIR}/lcov-1.11
export PATH=${LCOV_ROOT_DIR}/bin:${PATH}
##

#------ OCCT ------
# Defaults
export CSF_PluginDefaults=${CAS_ROOT_DIR}/src/StdResource
export CSF_StandardDefaults=${CAS_ROOT_DIR}/src/StdResource
# For CMake
export LIB=${LD_LIBRARY_PATH}
export PATH=${CASROOT}:${PATH}
##

#------ NewGEOM ------
export PATH=${NEWGEOM_ROOT_DIR}/bin:${NEWGEOM_ROOT_DIR}/plugins:${PATH}
export PYTHONPATH=${NEWGEOM_ROOT_DIR}/swig:${NEWGEOM_ROOT_DIR}/plugins:${NEWGEOM_ROOT_DIR}/addons:${NEWGEOM_ROOT_DIR}/pythonAPI:${PYTHONPATH}
export LD_LIBRARY_PATH=${NEWGEOM_ROOT_DIR}/bin:${NEWGEOM_ROOT_DIR}/swig:${NEWGEOM_ROOT_DIR}/plugins:${LD_LIBRARY_PATH}
export NEW_GEOM_CONFIG_FILE=${NEWGEOM_ROOT_DIR}/plugins
export NewGeomResources=${NEWGEOM_ROOT_DIR}/resources
