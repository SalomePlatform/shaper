#!/bin/bash -x

a_dir=$(dirname $0)

cd ${a_dir}

source ${a_dir}/env.sh
source ${a_dir}/env_salome.sh

SALOME_ARGS="-r ./test.squish/shared/testdata/SalomeApp.xml"
if [ -f GDB ]; then
  echo "No --gdb-session"
  #SALOME_ARGS="${SALOME_ARGS} --gdb-session"
fi
SALOME_ARGS="${SALOME_ARGS} --ns-port-log=$(pwd)/.salomeport"

# kill processes of previous SALOME run
if [ -f .salomeport ]; then
  SALOME_PORT=$(cat .salomeport)
  ${KERNEL_ROOT_DIR}/bin/salome/killSalomeWithPort.py ${SALOME_PORT}
  pkill -f "omniNames -start ${SALOME_PORT}"
  rm .salomeport
fi 

${KERNEL_ROOT_DIR}/bin/salome/runSalome.py ${SALOME_ARGS} >log_runSalome 2>err_runSalome &
SALOME_PID=$!

ATTEMPT=0
while [[ ! -f .salomeport && ${ATTEMPT} -lt 5 ]]; do
  sleep 5
  SALOME_PORT=$(cat .salomeport)
  ATTEMPT=$[${ATTEMPT}+1]
done 
if [ ${ATTEMPT} = 5 ]; then 
  echo "Can not receive SALOME_PORT: file .salomeport does not exist..."
  exit 1
fi

echo "SALOME_PORT ${SALOME_PORT}"

if [ -f GDB ]; then
  SALOME_SESSION_SERVER_PID="$(salome_pid.py ${SALOME_PORT} 'SALOME_Session_Server' 2>/dev/null)"
  ATTEMPT=0
  while [[ "${SALOME_SESSION_SERVER_PID}" = "" && "${ATTEMPT}" -lt 5 ]]; do
    sleep 5
    SALOME_SESSION_SERVER_PID="$(salome_pid.py ${SALOME_PORT} 'SALOME_Session_Server' 2>/dev/null)"
    ATTEMPT=$[${ATTEMPT}+1]
  done
  if [ ${ATTEMPT} = 5 ]; then 
    echo "Can not receive SALOME_SESSION_SERVER_PID..."
    exit 2
  fi

  echo "SALOME_SESSION_SERVER_PID ${SALOME_SESSION_SERVER_PID}"
  echo "Connecting GDB to PID ${SALOME_SESSION_SERVER_PID}..."
  gdb - ${SALOME_SESSION_SERVER_PID} >>log_gdb_salome 2>>err_gdb_salome
fi

wait ${SALOME_PID}

echo "Kill SALOME on port ${SALOME_PORT}"
${KERNEL_ROOT_DIR}/bin/salome/killSalomeWithPort.py ${SALOME_PORT}
pkill -f "omniNames -start ${SALOME_PORT}"

