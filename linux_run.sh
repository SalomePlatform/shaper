#!/bin/bash

# Find absolute path to ROOT_DIR
export ROOT_DIR=$(pwd)/..
export ROOT_DIR=`cd "${ROOT_DIR}";pwd`

SRC_DIR=${ROOT_DIR}/sources

source ${SRC_DIR}/linux_env.sh

${ROOT_DIR}/install/bin/GeomApp
