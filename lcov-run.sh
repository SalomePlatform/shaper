#!/bin/bash

source env.sh
source ${TOOLS_DIR}/env_linux.sh

echo "Collect coverage data..."
lcov --capture --directory ${BUILD_DIR} --output-file coverage.info

echo "Generate HTML for coverage data..."
genhtml coverage.info --output-directory ${1:-lcov_html}

