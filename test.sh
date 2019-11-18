#!/bin/bash

source env.sh

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

export DISPLAY="localhost:0.0"

# check for __init__.py in the SHAPER's Python scripts directory
# for correct parsing PYTHONPATH and use the actual version of SHAPER
# instead of distributed with SALOME
if [[ ! -f ${SHAPER_PYTHON_SCRIPTS_DIR}/salome/__init__.py ]]; then
  touch ${SHAPER_PYTHON_SCRIPTS_DIR}/salome/__init__.py
fi

if [[ $# > 0 ]]; then
  ctest --no-compress-output -T Test "$@" -R $1
else
  ctest --no-compress-output -T Test "$@"
fi
