#!/bin/bash

a_dir=$(dirname $0)

cd ${a_dir}

source ${a_dir}/env.sh
source ${a_dir}/env_standalone.sh

${INSTALL_DIR}/bin/GeomApp "$@"
