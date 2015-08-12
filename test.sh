#!/bin/bash

source env.sh
source env_standalone.sh

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

ctest --no-compress-output -T Test
