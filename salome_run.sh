#!/bin/bash -e

a_dir=$(dirname $0)

cd ${a_dir}

#if [ "$#" = 1 ]; then
#  export SALOME_PORT="$1"
#elif [ -z ${SALOME_PORT} ]; then
#  export SALOME_PORT=2900
#fi

echo "Run SALOME on port ${SALOME_PORT}"

source ${a_dir}/env.sh
source ${a_dir}/env_salome.sh

set -x +e

SALOME_ARGS="-r ./test.squish/shared/testdata/SalomeApp.xml"
#SALOME_ARGS="${SALOME_ARGS} --port=${SALOME_PORT}"
if [ -f GDB ]; then
  echo "No --gdb-session"
  #SALOME_ARGS="${SALOME_ARGS} --gdb-session"
fi
SALOME_ARGS="${SALOME_ARGS} --ns-port-log=$(pwd)/.salomeport"

${KERNEL_ROOT_DIR}/bin/salome/runSalome.py ${SALOME_ARGS} 2>&1 >/dev/null &
SALOME_PID=$!
wait ${SALOME_PID}

echo "SALOME_PORT=$(cat $(pwd)/.salomeport)"

${KERNEL_ROOT_DIR}/bin/salome/killSalomeWithPort.py ${SALOME_PORT}
