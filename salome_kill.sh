#!/bin/bash -e

a_dir=$(dirname $0)

source ${a_dir}/env.sh
source ${a_dir}/env_salome.sh

set +e -x

${KERNEL_ROOT_DIR}/bin/salome/killSalome.py
killall SALOME_LauncherServer
killall SALOME_Session_Server
killall SALOME_ConnectionManagerServer
killall omniNames
