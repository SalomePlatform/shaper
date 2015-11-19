#!/bin/bash

source env.sh
source ${TOOLS_DIR}/env_linux.sh

echo "Reset coverage data..."
lcov --zerocounters --directory $(cd ../; pwd)
