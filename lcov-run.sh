#!/bin/bash

source env.sh
source ${TOOLS_DIR}/env_linux.sh

export GCOV_DIR=${GCOV_DIR:-${BUILD_DIR}}

echo "Collect coverage data..."
lcov --capture --directory ${GCOV_DIR} --output-file coverage.info

echo "Generate HTML for coverage data..."
genhtml coverage.info --output-directory ${1:-lcov_html}

