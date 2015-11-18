#!/bin/bash -ex

while [[ $# > 0 ]]; do
  key="$1"

  case $key in
    standalone|Standalone)
    MODE=Standalone
    shift
    ;;
    *)
    shift
    ;;
  esac
done


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

include_products() {
  local salome_version=$1
  local platform="$(lsb_release -is)"
  local release="$(lsb_release -rs)"
  if [[ ${salome_version} = '7.7.0' ]]; then
    if   [[ "${platform}" = 'CentOS' ]]; then export PRODUCTS_ROOT=/dn23/NEWGEOM/common/PRODUCTS/CentOS.6.3.64/7.7.0
    elif [[ "${platform}" = 'Debian' && ${release} =~ ^6 ]]; then export PRODUCTS_ROOT=/dn23/NEWGEOM/common/PRODUCTS/DEBIAN.6.0.64/7.7.0
    fi
  else
    echo "Could not find PRODUCTS for the platform ${platform} ${release}"
    exit 1
  fi
}
include_products '7.7.0'

include_suit() {
  local salome_version=$1
  local platform="$(lsb_release -is)"
  local release="$(lsb_release -rs)"
  if [[ ${salome_version} = '7.7.0' ]]; then
    if   [[ "${platform}" = 'CentOS' ]]; then export SUIT_DIR=/dn46/SALOME/series7x/V7_7_0/CentOS63-64/GUI
    elif [[ "${platform}" = 'Debian' && ${release} =~ ^6 ]]; then export SUIT_DIR=/dn46/SALOME/series7x/V7_7_0/Debian60-64/GUI
    fi
  else
    echo "Could not find PRODUCTS for the platform ${platform} ${release}"
    exit 1
  fi
}
include_suit '7.7.0'

include_salome() {
  local salome_version=$1
  local platform="$(lsb_release -is)"
  local release="$(lsb_release -rs)"
  if [[ ${salome_version} = '7.5.1' ]]; then
    if   [[ "${platform}" = 'CentOS' ]]; then export SALOME_ROOT=/dn48/newgeom/common/SALOME-7.5.1_CentOS-6.3
    elif [[ "${platform}" = 'Debian' && ${release} =~ ^6 ]]; then export SALOME_ROOT=/dn23/NEWGEOM/common/SALOME-7.5.1_Debian-6.0_SOURCES
    elif [[ "${platform}" = 'Debian' && ${release} =~ ^8 ]]; then export SALOME_ROOT=/dn23/NEWGEOM/common/SALOME-7.5.1_Debian-8.0_SOURCES
    fi
  elif [[ ${salome_version} = '7.6.0' ]]; then
    if   [[ "${platform}" = 'CentOS' ]]; then export SALOME_ROOT=/dn23/NEWGEOM/common/SALOME-7.6.0_CentOS-6.3_SOURCES
    elif [[ "${platform}" = 'Debian' && ${release} =~ ^6 ]]; then export SALOME_ROOT=/dn23/NEWGEOM/common/SALOME-7.6.0_Debian-6.0_SOURCES
    fi
  elif [[ ${salome_version} = '7.7.0' ]]; then
    if   [[ "${platform}" = 'CentOS' ]]; then export SALOME_ROOT=/dn23/NEWGEOM/common/SALOME-7.7.0_CentOS63-64
    elif [[ "${platform}" = 'Debian' && ${release} =~ ^6 ]]; then export SALOME_ROOT=/dn23/NEWGEOM/common/SALOME-7.7.0_Debian60-64
    fi
  else 
    echo "Could not find SALOME for the platform ${platform} ${release}"
    exit 1
  fi
}
include_salome '7.7.0'


if [[ ${MODE} = 'Standalone' ]]; then
  echo "export INST_ROOT=${PRODUCTS_ROOT}" >> env_config.sh
  echo "export SUIT_DIR=${SUIT_DIR}" >> env_config.sh
else
  echo "export INST_ROOT=${SALOME_ROOT}" >> env_config.sh
fi

