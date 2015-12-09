#!/bin/bash

source env.sh
source ${TOOLS_DIR}/env_salome.sh

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

ctest --no-compress-output -T Test "$@"
