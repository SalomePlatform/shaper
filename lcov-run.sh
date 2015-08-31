#!/bin/bash

source env_lcov.sh

echo "Collect coverage data..."
lcov --capture --directory ../ --output-file coverage.info

echo "Generate HTML for coverage data..."
genhtml coverage.info --output-directory lcov_html

