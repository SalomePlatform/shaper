#!/bin/bash

set +u
echo "Export SALOME from ${INST_ROOT}..."
source ${INST_ROOT}/env_products.sh
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
export NEWGEOM_ROOT_DIR=${INSTALL_DIR}
export PATH=${NEWGEOM_ROOT_DIR}/bin:${NEWGEOM_ROOT_DIR}/plugins:${PATH}
export PYTHONPATH=${NEWGEOM_ROOT_DIR}/swig:${NEWGEOM_ROOT_DIR}/plugins:${NEWGEOM_ROOT_DIR}/addons:${NEWGEOM_ROOT_DIR}/PythonAPI:${PYTHONPATH}
export LD_LIBRARY_PATH=${NEWGEOM_ROOT_DIR}/bin:${NEWGEOM_ROOT_DIR}/swig:${NEWGEOM_ROOT_DIR}/plugins:${LD_LIBRARY_PATH}
export NEW_GEOM_CONFIG_FILE=${NEWGEOM_ROOT_DIR}/plugins
export NewGeomResources=${NEWGEOM_ROOT_DIR}/resources
