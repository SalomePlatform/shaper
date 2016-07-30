#!/bin/bash

source env.sh

export PATH=${SHAPER_ROOT_DIR}/bin:${PATH}

cd ${BUILD_DIR}

if [[ -n $@ ]]; then
  $@
else
  bash
fi
