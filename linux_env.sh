#------ Setting products installation directory ------

if [ "${PDIR}" = "" ] ; then
    export PDIR=/dn48/newgeom/common/SALOME-7.4.0
fi

# Find absolute path to ROOT_DIR
export ROOT_DIR=$(pwd)/..
export ROOT_DIR=`cd "${ROOT_DIR}";pwd`

#------ Python ------
export PYTHONHOME=${PDIR}/Python-2.7.3
export PYTHON_INCLUDE=${PYTHONHOME}/include/python2.7
export PYTHON_VERSION=2.7
export PATH=${PYTHONHOME}/bin:${PATH}
export LD_LIBRARY_PATH=${PYTHONHOME}/lib:${LD_LIBRARY_PATH}
export PYTHONPATH=${PYTHONHOME}/lib/python2.7
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
export FREEIMAGE_ROOT_DIR=${PDIR}/freeimage-3.16.0
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
export LIBXML_DIR=${PDIR}/libxml2-2.9.0
export PATH=${LIBXML_DIR}/bin:${PATH}
export LD_LIBRARY_PATH=${LIBXML_DIR}/lib:${LD_LIBRARY_PATH}
##
#------ SolveSpace ------
export SOLVESPACE_ROOT_DIR=${PDIR}/solvespace-2.0
export LD_LIBRARY_PATH=${SOLVESPACE_ROOT_DIR}/lib:${LD_LIBRARY_PATH}
##
#------ OCCT ------
export CAS_ROOT_DIR=${PDIR}/OCCT-6.7.1

#------ lcov ------
export LCOV_ROOT_DIR=${PDIR}/lcov-1.11
export PATH=${LCOV_ROOT_DIR}/bin:${PATH}

#------ SUIT ------
export SUIT_DIR=${PDIR}/GUI_7.4.0
export PATH=${SUIT_DIR}/bin/salome:${PATH}
export LD_LIBRARY_PATH=${SUIT_DIR}/lib/salome:${LD_LIBRARY_PATH}

# CAS_ROOT_DIR has to be in the PATH for CMake's FindCAS procedures
export PATH=${CAS_ROOT_DIR}:${CAS_ROOT_DIR}/bin:${PATH}
export LD_LIBRARY_PATH=${CAS_ROOT_DIR}/lib:${LD_LIBRARY_PATH}
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
# For CMake
export LIB=${LD_LIBRARY_PATH}

#------ NewGEOM ------
export NEW_GEOM_ROOT_DIR=${ROOT_DIR}/install
export PATH=${NEW_GEOM_ROOT_DIR}/bin:${NEW_GEOM_ROOT_DIR}/plugins:${PATH}
export PYTHONPATH=${NEW_GEOM_ROOT_DIR}/swig:${PYTHONPATH}
export LD_LIBRARY_PATH=${NEW_GEOM_ROOT_DIR}/bin:${NEW_GEOM_ROOT_DIR}/swig:${NEW_GEOM_ROOT_DIR}/plugins:${LD_LIBRARY_PATH}
export NEW_GEOM_CONFIG_FILE=${NEW_GEOM_ROOT_DIR}/plugins
export NewGeomResources=${NEW_GEOM_ROOT_DIR}/resources

