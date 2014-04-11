
# Find absolute path to ROOT_DIR
export ROOT_DIR=$(pwd)/..
export ROOT_DIR=`cd "${ROOT_DIR}";pwd`

SRC_DIR=${ROOT_DIR}/sources

source ${SRC_DIR}/linux_env.sh
mkdir -p ${ROOT_DIR}/build
cd ${ROOT_DIR}/build

CMAKE_ARGS="-D_ECLIPSE_VERSION=4.3"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Debug"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=ON"
CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX:PATH=${ROOT_DIR}/install"
CMAKE_ARGS="${CMAKE_ARGS} ${SRC_DIR}"

cmake -G "Eclipse CDT4 - Unix Makefiles" ${CMAKE_ARGS}

/misc/dn21/hdfeditor/sbh/devtools/eclipse-4.3.0/eclipse&