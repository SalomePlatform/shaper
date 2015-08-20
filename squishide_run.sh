#!/bin/bash -x

a_dir=$(dirname $0)

source ${a_dir}/env_squish.sh

export TEST_DATA_DIR=$(cd ${a_dir}; pwd)/test.squish/shared/testdata

squishide
