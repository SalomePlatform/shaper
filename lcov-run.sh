#!/bin/bash

source env_lcov.sh

echo "Collect coverage data..."
lcov --capture --directory $(cd ../; pwd) --output-file coverage.info --no-external

echo "Generate HTML for coverage data..."
genhtml coverage.info --output-directory lcov_html

