#!/bin/bash

a_dir=$(dirname $0)

source ${a_dir}/env.sh
source ${a_dir}/env_standalone.sh

${INSTALL_DIR}/bin/GeomApp "$@"
