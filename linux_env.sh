#!/bin/sh

if [ "${INST_ROOT}" = "" ] ; then
    source /dn48/newgeom/common/SALOME/env_products.sh
else
    source ${INST_ROOT}/env_products.sh
fi

if [ "${NEWGEOM_PDIR}" = "" ] ; then
    export NEWGEOM_PDIR=/dn48/newgeom/common/products
fi

if [ "${ROOT_DIR}" = "" ] ; then
  export ROOT_DIR=$(pwd)/..
  export ROOT_DIR=`cd "${ROOT_DIR}";pwd`
fi

#------ SolveSpace ------
export SOLVESPACE_ROOT_DIR=${NEWGEOM_PDIR}/solvespace-2.0
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
export NEWGEOM_ROOT_DIR=${ROOT_DIR}/install
export PATH=${NEWGEOM_ROOT_DIR}/bin:${NEWGEOM_ROOT_DIR}/plugins:${PATH}
export PYTHONPATH=${NEWGEOM_ROOT_DIR}/swig:${NEWGEOM_ROOT_DIR}/plugins:${PYTHONPATH}
export LD_LIBRARY_PATH=${NEWGEOM_ROOT_DIR}/bin:${NEWGEOM_ROOT_DIR}/swig:${NEWGEOM_ROOT_DIR}/plugins:${LD_LIBRARY_PATH}
export NEW_GEOM_CONFIG_FILE=${NEWGEOM_ROOT_DIR}/plugins
export NewGeomResources=${NEWGEOM_ROOT_DIR}/resources

