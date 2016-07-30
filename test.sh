#!/bin/bash

source env.sh

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

ctest --no-compress-output -T Test "$@"
