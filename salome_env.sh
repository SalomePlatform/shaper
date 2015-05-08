#!/bin/bash

main() {
  local platform="$(lsb_release -is)"
  local release="$(lsb_release -rs)"
  if [ "${platform}" == "CentOS" ]; then 
    export INST_ROOT=/dn48/newgeom/common/SALOME-7.5.1_CentOS-6.3
  elif [ "${platform}" == "Debian" ]; then
    if   [[ ${release} =~ ^6 ]]; then export INST_ROOT=/dn23/NEWGEOM/common/SALOME-7.5.1_Debian-6.0_SOURCES
    elif [[ ${release} =~ ^8 ]]; then export INST_ROOT=/dn23/NEWGEOM/common/SALOME-7.5.1_Debian-8.0_SOURCES
    fi
  fi
}

main
