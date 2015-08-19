#!/bin/bash -ex

source env.sh
source env_salome.sh

${KERNEL_ROOT_DIR}/bin/salome/killSalome.py
set +e
killall SALOME_LauncherServer
killall SALOME_Session_Server
killall SALOME_ConnectionManagerServer
set -e
