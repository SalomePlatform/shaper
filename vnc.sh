#!/bin/bash -e

if [ -z ${DISPLAY_PORT} ]; then
  export DISPLAY_PORT=99
fi

echo "DISPLAY is :${DISPLAY_PORT}"

export PATH=/dn23/NEWGEOM/NEWGEOM_JENKINS_BUILD_AREA/tools:/opt/TurboVNC/bin:${PATH}
./vnc-run --server-num ${DISPLAY_PORT} -e vnc.err vglrun "$@"
