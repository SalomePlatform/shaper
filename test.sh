#!/bin/bash

source env.sh

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

export DISPLAY="localhost:0.0"

if [[ $# > 0 ]]; then
  ctest --no-compress-output -T Test "$@" -R $1
else
  ctest --no-compress-output -T Test "$@"
fi
