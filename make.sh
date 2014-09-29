
# Find absolute path to ROOT_DIR
export ROOT_DIR=$(pwd)/..
export ROOT_DIR=`cd "${ROOT_DIR}";pwd`

SRC_DIR=${ROOT_DIR}/sources

source ${SRC_DIR}/linux_env.sh
mkdir -p ${ROOT_DIR}/build
cd ${ROOT_DIR}/build

CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Debug"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX:PATH=${ROOT_DIR}/install"
CMAKE_ARGS="${CMAKE_ARGS} ${SRC_DIR}"

cmake -G "Unix Makefiles" ${CMAKE_ARGS}
make
make install
