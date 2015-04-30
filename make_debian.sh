
# Find absolute path to ROOT_DIR
export ROOT_DIR=$(pwd)/..
export ROOT_DIR=`cd "${ROOT_DIR}";pwd`

SRC_DIR=${ROOT_DIR}/sources

export INST_ROOT=/misc/dn48/newgeom/common/SALOME-7.5.1_Debian-6.0
source ${SRC_DIR}/linux_env.sh
mkdir -p ${ROOT_DIR}/build-debian
cd ${ROOT_DIR}/build-debian

CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Release"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX:PATH=${ROOT_DIR}/install"
CMAKE_ARGS="${CMAKE_ARGS} ${SRC_DIR}"

cmake -G "Unix Makefiles" ${CMAKE_ARGS}
make
make install
