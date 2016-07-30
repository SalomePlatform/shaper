#!/bin/bash

source env.sh

cd ${BUILD_DIR}

if [[ -n $@ ]]; then
  $@
else
  bash
fi
