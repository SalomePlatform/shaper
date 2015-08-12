#!/bin/bash

main() {
  local salome_version=7.6.0
  local platform="$(lsb_release -is)"
  local release="$(lsb_release -rs)"
  if [[ ${salome_version} = '7.5.1' ]]; then
    if   [[ "${platform}" = 'CentOS' ]]; then export INST_ROOT=/dn48/newgeom/common/SALOME-7.5.1_CentOS-6.3
    elif [[ "${platform}" = 'Debian' && ${release} =~ ^6 ]]; then export INST_ROOT=/dn23/NEWGEOM/common/SALOME-7.5.1_Debian-6.0_SOURCES
    elif [[ "${platform}" = 'Debian' && ${release} =~ ^8 ]]; then export INST_ROOT=/dn23/NEWGEOM/common/SALOME-7.5.1_Debian-8.0_SOURCES
    fi
  elif [[ ${salome_version} = '7.6.0' ]]; then
    if   [[ "${platform}" = 'CentOS' ]]; then export INST_ROOT=/dn23/NEWGEOM/common/SALOME-7.6.0_CentOS-6.3_SOURCES
    elif [[ "${platform}" = 'Debian' && ${release} =~ ^6 ]]; then export INST_ROOT=/dn23/NEWGEOM/common/SALOME-7.6.0_Debian-6.0_SOURCES
    fi
  fi
}

main

