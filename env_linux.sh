#!/bin/bash

# This script uses:
#
# INST_ROOT - path of SALOME (env_products.sh)
# SHAPER_PDIR - path of PRODUCTS for SHAPER (SolveSpace, lcov)
# SHAPER_ROOT_DIR - path of SHAPER installation

for path in INST_ROOT SHAPER_PDIR SHAPER_ROOT_DIR; do
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
export SOLVESPACE_ROOT_DIR=${SHAPER_PDIR}/solvespace-2.1
export LD_LIBRARY_PATH=${SOLVESPACE_ROOT_DIR}/lib:${LD_LIBRARY_PATH}
##

#------ PlaneGCS ------
export PLANEGCS_ROOT_DIR=${SHAPER_PDIR}/planegcs-0.16
export LD_LIBRARY_PATH=${PLANEGCS_ROOT_DIR}/lib:${LD_LIBRARY_PATH}
##

#------ Eigen ------
export EIGEN_ROOT_DIR=${SHAPER_PDIR}/eigen-3.2.7
##

#------ Boost ------
export BOOST_ROOT_DIR=${SHAPER_PDIR}/boost-1.52.0
##

#------ lcov ------
export LCOV_ROOT_DIR=${SHAPER_PDIR}/lcov-1.11
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

#------ SHAPER ------
export PATH=${SHAPER_ROOT_DIR}/bin/salome:${SHAPER_ROOT_DIR}/lib/salome:${PATH}
export PYTHONPATH=${SHAPER_ROOT_DIR}//bin/salome:${SHAPER_ROOT_DIR}/lib/salome:${PYTHONPATH}
export LD_LIBRARY_PATH=${SHAPER_ROOT_DIR}/bin/salome:${SHAPER_ROOT_DIR}/lib/salome:${LD_LIBRARY_PATH}
export SHAPER_CONFIG_FILE=${SHAPER_ROOT_DIR}/share/salome/resources/shaper
export SHAPERResources=${SHAPER_ROOT_DIR}/share/salome/resources/shaper
