#!/bin/sh

if [ "${INST_ROOT}" = "" ] ; then
    source /dn47/SALOME/PRODUCTS/7x/opt/CentOS.6.3.64/7.3.0/env_products.sh
    source /dn47/SALOME/stable/V7_3_0/start.sh
else
    source ${INST_ROOT}/env_products.sh
fi


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
