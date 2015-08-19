#!/bin/bash

cd $(dirname $0)

source env.sh
source env_standalone.sh

${INSTALL_DIR}/bin/GeomApp "$@"
