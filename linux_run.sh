#!/bin/bash

a_dir=$(dirname $0)

cd ${a_dir}

source ${a_dir}/env.sh
source ${a_dir}/env_standalone.sh

if [ -f GDB ]; then
  ${INSTALL_DIR}/bin/GeomApp >>log_GeomApp 2>>err_GeomApp &
  APP_PID=$!
  echo "Connecting GDB to PID ${APP_PID}..."
  gdb - ${APP_PID} >>log_gdb_linux 2>>err_gdb_linux
else
  ${INSTALL_DIR}/bin/GeomApp
fi
