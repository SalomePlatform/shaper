#!/bin/bash

source env.sh
source env_standalone.sh

cd ${BUILD_DIR}

if [[ -n $@ ]]; then
  $@
else
  bash
fi
