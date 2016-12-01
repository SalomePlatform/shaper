#!/bin/bash

# This script uses:
#
# SHAPER_PDIR - path of PRODUCTS for SHAPER (SolveSpace, lcov, cpplint)
# SHAPER_ROOT_DIR - path of SHAPER installation

for path in SHAPER_PDIR SHAPER_ROOT_DIR; do
  if [[ -z ${!path+x} ]]; then
    echo "${path} not found."; exit 1
  else
    echo "Found ${path}: ${!path}"
  fi
done

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

#------ cpplint ------
export CPPLINT_ROOT_DIR=${SHAPER_PDIR}/cpplint-1.3.0
export PATH=${CPPLINT_ROOT_DIR}:${PATH}
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
export PATH=${SHAPER_ROOT_DIR}/bin/salome:${PATH}
export PYTHONPATH=${SHAPER_ROOT_DIR}/bin/salome:${PYTHONPATH}
export LD_LIBRARY_PATH=${SHAPER_ROOT_DIR}/lib/salome:${LD_LIBRARY_PATH}
