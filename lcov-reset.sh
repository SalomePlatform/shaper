#!/bin/bash

source env_lcov.sh

echo "Reset coverage data..."
lcov --zerocounters --directory $(cd ../; pwd)
