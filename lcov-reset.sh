#!/bin/bash

source env.sh
source ${TOOLS_DIR}/env_linux.sh

export GCOV_DIR=${GCOV_DIR:-${BUILD_DIR}}

echo "Reset coverage data..."
lcov --zerocounters --directory ${GCOV_DIR}

