#!/bin/bash

a_dir=$(dirname $0)

cd ${a_dir}

${a_dir}/vnc.sh ${a_dir}/test_squish.sh

