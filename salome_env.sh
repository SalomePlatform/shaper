#!/bin/sh

if [ "${INST_ROOT}" = "" ] ; then
    source /dn48/newgeom/common/SALOME/env_products.sh
else
    source ${INST_ROOT}/env_products.sh
fi


#------ SolveSpace ------
export SOLVESPACE_ROOT_DIR=/dn48/newgeom/common/products/solvespace-2.0
export LD_LIBRARY_PATH=${SOLVESPACE_ROOT_DIR}/lib:${LD_LIBRARY_PATH}
##

export ROOT_DIR=$(pwd)/..
export ROOT_DIR=`cd "${ROOT_DIR}";pwd`

SRC_DIR=${ROOT_DIR}/sources

export LIB=${LD_LIBRARY_PATH}
export PATH=${CASROOT}:${PATH}
export PYTHON_INC_DIR=${PYTHON_INCLUDE}
export PYTHON_LIB_DIR=${PYTHON_ROOT_DIR}/lib


#------ NewGEOM ------
export INST_DIR=${ROOT_DIR}/install
export PATH=${INST_DIR}/bin:${INST_DIR}/plugins:${PATH}
export LD_LIBRARY_PATH=${INST_DIR}/bin:${INST_DIR}/swig:${INST_DIR}/plugins:${PATH}${LD_LIBRARY_PATH}
export NEW_GEOM_CONFIG_FILE=${INST_DIR}/plugins
