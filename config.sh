#!/bin/bash -ex

rm env_config.sh || echo "File env_config.sh was not found."

echo "#!/bin/bash -ex" >> env_config.sh

PLATFORM=""
if [ -f PLATFORM ]; then
  PLATFORM="$(lsb_release -is)"
  RELEASE="$(lsb_release -rs)"
  if [ "${PLATFORM}" = 'CentOS' ]; then PLATFORM=-centos
  elif [ "${PLATFORM}" = 'Debian' ]; then
    if [ "${RELEASE}" = '8.0' ]; then PLATFORM=-d8
    else PLATFORM=-d6
    fi
  fi
fi

if [ -f DEBUG ]; then
  PLATFORM=${PLATFORM}_d
fi

echo "export PLATFORM=${PLATFORM}" >> env_config.sh

include_salome() {
  local salome_version=$1
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
  elif [[ ${salome_version} = '7.7.0' ]]; then
    if   [[ "${platform}" = 'CentOS' ]]; then export INST_ROOT=/dn23/NEWGEOM/common/SALOME-7.7.0_CentOS63-64
    elif [[ "${platform}" = 'Debian' && ${release} =~ ^6 ]]; then export INST_ROOT=/dn23/NEWGEOM/common/SALOME-7.7.0_Debian60-64
    fi
  else 
    echo "Could not find SALOME for the platform ${platform} ${release}"
    exit 1
  fi
}
include_salome '7.7.0'
echo "export INST_ROOT=${INST_ROOT}" >> env_config.sh

