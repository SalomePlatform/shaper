#!/bin/bash

source env.sh
source env_salome.sh

cd ${BUILD_DIR}

if [[ -n $@ ]]; then
  $@
else
  bash
fi
