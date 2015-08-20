#!/bin/bash -e

a_dir=$(dirname $0)

cd ${a_dir}

if [ "$#" = 1 ]; then
  export SALOME_PORT="$1"
elif [ -z ${SALOME_PORT} ]; then
  export SALOME_PORT=2900
fi

echo "Run SALOME on port ${SALOME_PORT}"

source ${a_dir}/env.sh
source ${a_dir}/env_salome.sh

set -x +e

${a_dir}/salome_kill.sh

${KERNEL_ROOT_DIR}/bin/salome/runSalome.py --port=${SALOME_PORT} -r ./test.squish/shared/testdata/SalomeApp.xml 2>&1 >/dev/null
