#!/bin/bash -e

export PATH=/dn23/NEWGEOM/NEWGEOM_JENKINS_BUILD_AREA/tools:/opt/TurboVNC/bin:${PATH}
./vnc-run vglrun "$@"
