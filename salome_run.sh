#!/bin/bash -x

export SALOME_PORT=2820

source env.sh
source env_salome.sh

${KERNEL_ROOT_DIR}/bin/salome/killSalomeWithPort.py ${SALOME_PORT}
${KERNEL_ROOT_DIR}/bin/salome/runSalome.py --port ${SALOME_PORT}
sleep 5
${KERNEL_ROOT_DIR}/bin/salome/killSalomeWithPort.py ${SALOME_PORT}
