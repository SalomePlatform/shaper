#!/bin/bash

source env.sh
source ${TOOLS_DIR}/env_standalone.sh

${TOOLS_DIR}/solver.sh "$@"

APP=${NEWGEOM_ROOT_DIR}/bin/GeomApp

${APP}
