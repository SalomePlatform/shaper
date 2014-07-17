#------ Setting products installation directory ------

if [ "${PDIR}" = "" ] ; then
    export PDIR=/dn48/newgeom/common/products
fi

# Find absolute path to ROOT_DIR
export ROOT_DIR=$(pwd)/..
export ROOT_DIR=`cd "${ROOT_DIR}";pwd`

#------ Python ------
export PYTHON_ROOT_DIR=${PDIR}/Python-2.7.3
export PYTHONHOME=${PYTHON_ROOT_DIR}
export PYTHON_INCLUDE=${PYTHON_ROOT_DIR}/include/python2.7
export PYTHON_VERSION=2.7
export PATH=${PYTHON_ROOT_DIR}/bin:${PATH}
export LD_LIBRARY_PATH=${PYTHON_ROOT_DIR}/lib:${LD_LIBRARY_PATH}
export PYTHONPATH=${PYTHON_ROOT_DIR}/lib/python2.7
#For cmake
export PYTHON_LIB_DIR=${PYTHON_ROOT_DIR}/lib
export PYTHON_INC_DIR=${PYTHON_INCLUDE}
##
#------ Qt ------
export QT4_ROOT_DIR=${PDIR}/qt-4.8.4
export QT_PLUGIN_PATH=${QT4_ROOT_DIR}/plugins
export PATH=${QT4_ROOT_DIR}/bin:${PATH}
export LD_LIBRARY_PATH=${QT4_ROOT_DIR}/lib:${LD_LIBRARY_PATH}
##
#------ boost ------
export BOOST_ROOT_DIR=${PDIR}/boost-1.52.0
export LD_LIBRARY_PATH=${BOOST_ROOT_DIR}/lib:${LD_LIBRARY_PATH}
##
#------ swig ------
export SWIG_ROOT_DIR=${PDIR}/swig-2.0.8
export SWIG_LIB=${SWIG_ROOT_DIR}/share/swig/2.0.8
export PATH=${SWIG_ROOT_DIR}/bin:${PATH}
##
#------ cmake ------
export CMAKE_ROOT_DIR=${PDIR}/cmake-2.8.10.2
export PATH=${CMAKE_ROOT_DIR}/bin:${PATH}
##
#------ freetype ------
export FREETYPE_ROOT_DIR=${PDIR}/freetype-2.4.11
export LD_LIBRARY_PATH=${FREETYPE_ROOT_DIR}/lib:${LD_LIBRARY_PATH}
##
#------ freeimage ------
export FREEIMAGE_ROOT_DIR=${PDIR}/freeimage-3.15.4
export PATH=${FREEIMAGE_ROOT_DIR}/bin:${PATH}
export LD_LIBRARY_PATH=${FREEIMAGE_ROOT_DIR}/lib:${LD_LIBRARY_PATH}
##
#------ gl2ps ------
export GL2PS_ROOT_DIR=${PDIR}/gl2ps-1.3.8
export PATH=${GL2PS_ROOT_DIR}/bin:${PATH}
export LD_LIBRARY_PATH=${GL2PS_ROOT_DIR}/lib:${LD_LIBRARY_PATH}
##
#------ tbb ------
export TBB_ROOT_DIR=${PDIR}/tbb-30_018oss
export PATH=${TBB_ROOT_DIR}/bin/intel64/cc4.1.0_libc2.4_kernel2.6.16.21:${PATH}
export LD_LIBRARY_PATH=${TBB_ROOT_DIR}/lib/intel64/cc4.1.0_libc2.4_kernel2.6.16.21:${LD_LIBRARY_PATH}
##
#------ libxml2 ------
export LIBXML2_ROOT_DIR=${PDIR}/libxml2-2.9.0
export PATH=${LIBXML2_ROOT_DIR}/bin:${PATH}
export LD_LIBRARY_PATH=${LIBXML2_ROOT_DIR}/lib:${LD_LIBRARY_PATH}
# DO NOT rename following 3 variables, 
# they are required by CMake "find(Libxml2)" procedure
export LIBXML2_INCLUDE_DIR=${LIBXML2_ROOT_DIR}/include/libxml2
export LIBXML2_LIB_DIR=${LIBXML2_ROOT_DIR}/lib
export LIBXML2_BIN_DIR=${LIBXML2_ROOT_DIR}/bin
##
#------ SolveSpace ------
export SOLVESPACE_ROOT_DIR=${PDIR}/solvespace-2.0
export LD_LIBRARY_PATH=${SOLVESPACE_ROOT_DIR}/lib:${LD_LIBRARY_PATH}
##
#------ OCCT ------
export CAS_ROOT_DIR=${PDIR}/OCCT-6.7.0

export PATH=${CAS_ROOT_DIR}:${CAS_ROOT_DIR}/bin:${PATH}
export LD_LIBRARY_PATH=${CAS_ROOT_DIR}:${CAS_ROOT_DIR}/lib:${LD_LIBRARY_PATH}
export CSF_GraphicShr=${CAS_ROOT_DIR}/lib/libTKOpenGl.so
# Variable for Foundation Classes : 
export CSF_UnitsLexicon=${CAS_ROOT_DIR}/src/UnitsAPI/Lexi_Expr.dat 
export CSF_UnitsDefinition=${CAS_ROOT_DIR}/src/UnitsAPI/Units.dat 
# Variable for DataExchange : 
export CSF_SHMessage=${CAS_ROOT_DIR}/src/SHMessage
export CSF_XSMessage=${CAS_ROOT_DIR}/src/XSMessage 
# Variable for Font : 
export CSF_MDTVFontDirectory=${CAS_ROOT_DIR}/src/FontMFT 
export CSF_MDTVTexturesDirectory=${CAS_ROOT_DIR}/src/Textures
# Defaults
export CSF_PluginDefaults=${CAS_ROOT_DIR}/src/StdResource 
export CSF_StandardDefaults=${CAS_ROOT_DIR}/src/StdResource
# Activation of OCCT Kernel multithreading :
export MMGT_REENTRANT=1
# this variable only needed for DRAWEXE
export CASROOT=${CAS_ROOT_DIR}
##
export LIB=${LD_LIBRARY_PATH}

#------ lcov ------
export LCOV_ROOT_DIR=${PDIR}/lcov-1.11
export PATH=${LCOV_ROOT_DIR}/bin:${PATH}

#------ NewGEOM ------
export INST_DIR=${ROOT_DIR}/install
export PATH=${INST_DIR}/bin:${INST_DIR}/plugins:${PATH}
export PYTHONPATH=${INST_DIR}/swig:${PYTHONPATH}
export LD_LIBRARY_PATH=${INST_DIR}/bin:${INST_DIR}/swig:${INST_DIR}/plugins:${LD_LIBRARY_PATH}
export NEW_GEOM_CONFIG_FILE=${INST_DIR}/plugins
