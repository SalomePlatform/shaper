#!/bin/bash -ex

if [ "$#" = 1 ]; then
  export SALOME_PORT="$1"
else
  export SALOME_PORT=2900
fi

source env.sh
source env_salome.sh

${KERNEL_ROOT_DIR}/bin/salome/killSalomeWithPort.py ${SALOME_PORT}
${KERNEL_ROOT_DIR}/bin/salome/runSalome.py --port=${SALOME_PORT} -r ./test.squish/shared/testdata/SalomeApp.xml
sleep 5
${KERNEL_ROOT_DIR}/bin/salome/killSalomeWithPort.py ${SALOME_PORT}
