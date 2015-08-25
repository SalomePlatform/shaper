#!/bin/bash

source env.sh
source env_standalone.sh

export PATH=${INSTALL_DIR}/bin:${PATH}

cd ${BUILD_DIR}

if [[ -n $@ ]]; then
  $@
else
  bash
fi
