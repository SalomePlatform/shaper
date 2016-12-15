# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

IF(NOT CAS_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for Open CASCADE Technology ...")
ENDIF()

SET(_CAS_VERSION_FILE_NAME Standard_Version.hxx)

# Include directories:
FIND_PATH(CAS_INCLUDE_DIRS ${_CAS_VERSION_FILE_NAME} PATH_SUFFIXES include/opencascade)
FIND_FILE(CAS_VERSION_FILE ${_CAS_VERSION_FILE_NAME} PATH_SUFFIXES include/opencascade)
IF(CAS_INCLUDE_DIRS)
  SET(CAS_UPCOUNT 2)
ELSE()
  FIND_PATH(CAS_INCLUDE_DIRS ${_CAS_VERSION_FILE_NAME} PATH_SUFFIXES include inc)
  FIND_FILE(CAS_VERSION_FILE ${_CAS_VERSION_FILE_NAME} PATH_SUFFIXES include inc)
ENDIF()
MARK_AS_ADVANCED(CAS_VERSION_FILE)

SET(CAS_VERSION_DEVELOPMENT 0)
IF(CAS_VERSION_FILE)
  # Check if this is a development version:
  FILE(STRINGS ${CAS_VERSION_FILE} CAS_VERSION_DEVELOPMENT_STR
      REGEX "^ *#define OCC_VERSION_DEVELOPMENT")
  IF(CAS_VERSION_DEVELOPMENT_STR)
    SET(CAS_VERSION_DEVELOPMENT 1)
  ENDIF(CAS_VERSION_DEVELOPMENT_STR)
  
  # Extract actual version number:
  FILE(STRINGS ${CAS_VERSION_FILE} _tmp
      REGEX "^ *#define OCC_VERSION_COMPLETE *\"[^\"]*\".*$")
  STRING(REGEX REPLACE ".*\"([^\"]*)\".*" "\\1" CAS_VERSION_STR "${_tmp}")
ENDIF()

# Win specific stuff:
IF(WIN32)
  IF(CMAKE_BUILD_TYPE STREQUAL Debug)
    SET(CMAKE_LIBRARY_PATH ${CAS_ROOT_DIR}/win32/libd ${CAS_ROOT_DIR}/win64/lib/vc10)
  ELSE()
    SET(CMAKE_LIBRARY_PATH ${CAS_ROOT_DIR}/win32/lib ${CAS_ROOT_DIR}/win64/lib/vc10)
  ENDIF()
ENDIF()

# Definitions:
SET(CAS_DEFINITIONS "-DLIN -DLINTEL -DCSFDB")
SET(CAS_DEFINITIONS "${CAS_DEFINITIONS} -DNo_exception")

# Check config.h file
FIND_FILE(CAS_CONFIG_H config.h PATHS ${CAS_INCLUDE_DIRS} NO_DEFAULT_PATH)
IF(CAS_CONFIG_H)
 SET(CAS_DEFINITIONS "${CAS_DEFINITIONS} -DHAVE_CONFIG_H")
ENDIF()

# Test for 64 bit machine:
IF(CMAKE_SIZEOF_VOID_P STREQUAL 8)
  SET(CAS_DEFINITIONS "${CAS_DEFINITIONS} -D_OCC64")
ENDIF()

# Find Xmu library (X11 widgets?)
IF(NOT WIN32)
  FIND_LIBRARY(CAS_Xmu Xmu)
  IF(Xmu)
    SET(CAS_LDPATH ${Xmu})
  ENDIF()
ENDIF()

IF(CAS_VERSION_STR VERSION_LESS "7.0.1")
  FIND_LIBRARY(CAS_FWOSPlugin FWOSPlugin PATH_SUFFIXES lib lin64/gcc/lib bin)
ELSE()
  FIND_LIBRARY(CAS_TKStd TKStd PATH_SUFFIXES lib lin64/gcc/lib bin)
ENDIF()
FIND_LIBRARY(CAS_TKBin TKBin PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKBinL TKBinL PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKBinTObj TKBinTObj PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKBinXCAF TKBinXCAF PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKBO TKBO PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKBool TKBool PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKBRep TKBRep PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKCAF TKCAF PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKCDF TKCDF PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKernel TKernel PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKFeat TKFeat PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKFillet TKFillet PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKG2d TKG2d PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKG3d TKG3d PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKGeomAlgo TKGeomAlgo PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKGeomBase TKGeomBase PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKHLR TKHLR PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKIGES TKIGES PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKLCAF TKLCAF PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKMath TKMath PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKMesh TKMesh PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKMeshVS TKMeshVS PATH_SUFFIXES lib lin64/gcc/lib bin)
IF(CAS_VERSION_STR VERSION_LESS "6.9.1")
  FIND_LIBRARY(CAS_TKNIS TKNIS PATH_SUFFIXES lib lin64/gcc/lib bin)
ENDIF()
FIND_LIBRARY(CAS_TKOffset TKOffset PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKOpenGl TKOpenGl PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKPrim TKPrim PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKService TKService PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKShHealing TKShHealing PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKSTEP TKSTEP PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKSTEP209 TKSTEP209 PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKSTEPAttr TKSTEPAttr PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKSTEPBase TKSTEPBase PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKSTL TKSTL PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKTObj TKTObj PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKTopAlgo TKTopAlgo PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKV3d TKV3d PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKVRML TKVRML PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKXCAF TKXCAF PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKXDEIGES TKXDEIGES PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKXDESTEP TKXDESTEP PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKXMesh TKXMesh PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKXml TKXml PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKXmlL TKXmlL PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKXmlTObj TKXmlTObj PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKXmlXCAF TKXmlXCAF PATH_SUFFIXES lib lin64/gcc/lib bin)
FIND_LIBRARY(CAS_TKXSBase TKXSBase PATH_SUFFIXES lib lin64/gcc/lib bin)

IF(CAS_VERSION_STR VERSION_LESS "7.0")
  FIND_LIBRARY(CAS_PTKernel PTKernel PATH_SUFFIXES lib lin64/gcc/lib bin)
  FIND_LIBRARY(CAS_TKPCAF TKPCAF PATH_SUFFIXES lib lin64/gcc/lib bin)
  FIND_LIBRARY(CAS_TKPLCAF TKPLCAF PATH_SUFFIXES lib lin64/gcc/lib bin)
  FIND_LIBRARY(CAS_TKPShape TKPShape PATH_SUFFIXES lib lin64/gcc/lib bin)
  FIND_LIBRARY(CAS_TKShapeSchema TKShapeSchema PATH_SUFFIXES lib lin64/gcc/lib bin)
  FIND_LIBRARY(CAS_TKStdLSchema TKStdLSchema PATH_SUFFIXES lib lin64/gcc/lib bin)
  FIND_LIBRARY(CAS_TKStdSchema TKStdSchema PATH_SUFFIXES lib lin64/gcc/lib bin)
  FIND_LIBRARY(CAS_TKXCAFSchema TKXCAFSchema PATH_SUFFIXES lib lin64/gcc/lib bin)
ENDIF()

SET(CAS_KERNEL ${CAS_TKernel} ${CAS_TKMath})
SET(CAS_OCAF ${CAS_TKernel} ${CAS_TKMath} ${CAS_TKCDF} ${CAS_TKLCAF})
SET(CAS_VIEWER ${CAS_TKService} ${CAS_TKV3d} ${CAS_TKG3d} ${CAS_TKGeomBase} ${CAS_TKBRep})
SET(CAS_OCAFVIS ${CAS_TKCAF} ${CAS_TKBRep} ${CAS_TKG2d})
SET(CAS_MODELER ${CAS_TKG3d} ${CAS_TKGeomBase} ${CAS_TKGeomAlgo} ${CAS_TKBRep} ${CAS_TKTopAlgo} ${CAS_TKG2d})

SET(CAS_TKV3d_EA ${CAS_TKernel} ${CAS_TKMath} ${CAS_TKV3d})
SET(CAS_TKBool_EA ${CAS_TKernel} ${CAS_TKMath} ${CAS_TKBRep} ${CAS_TKG2d} ${CAS_TKG3d} ${CAS_TKGeomBase} ${CAS_TKGeomAlgo} ${CAS_TKTopAlgo} ${CAS_TKBool})
SET(CAS_TKBRep_EA ${CAS_TKernel} ${CAS_TKMath} ${CAS_TKBRep})
SET(CAS_TKIGES_EA ${CAS_TKernel} ${CAS_TKMath} ${CAS_TKXSBase} ${CAS_TKBRep} ${CAS_TKIGES})
SET(CAS_TKSTEP_EA ${CAS_TKernel} ${CAS_TKMath} ${CAS_TKXSBase} ${CAS_TKBRep} ${CAS_TKSTEP})
SET(CAS_TKSTL_EA ${CAS_TKernel} ${CAS_TKMath} ${CAS_TKSTL})
SET(CAS_TKCAF_EA ${CAS_TKPrim} ${CAS_TKCAF})

SET(CAS_TKV3d ${CAS_TKV3d_EA})
SET(CAS_TKBool ${CAS_TKBool_EA})
SET(CAS_TKBRep ${CAS_TKBRep_EA})
SET(CAS_TKIGES ${CAS_TKIGES_EA})
SET(CAS_TKSTEP ${CAS_TKSTEP_EA})
SET(CAS_TKSTL ${CAS_TKSTL_EA})
SET(CAS_TKCAF ${CAS_TKCAF_EA})

SET(_libs
  CAS_TKBin
  CAS_TKBinL
  CAS_TKBinTObj
  CAS_TKBinXCAF
  CAS_TKBO
  CAS_TKBool
  CAS_TKBRep
  CAS_TKCAF
  CAS_TKCDF
  CAS_TKernel
  CAS_TKFeat
  CAS_TKFillet
  CAS_TKG2d
  CAS_TKG3d
  CAS_TKGeomAlgo
  CAS_TKGeomBase
  CAS_TKHLR
  CAS_TKIGES
  CAS_TKLCAF
  CAS_TKMath
  CAS_TKMesh
  CAS_TKMeshVS
  CAS_TKOffset
  CAS_TKOpenGl
  CAS_TKPrim
  CAS_TKService
  CAS_TKShHealing
  CAS_TKSTEP
  CAS_TKSTEP209
  CAS_TKSTEPAttr
  CAS_TKSTEPBase
  CAS_TKSTL
  CAS_TKTObj
  CAS_TKTopAlgo
  CAS_TKV3d
  CAS_TKVRML
  CAS_TKXCAF
  CAS_TKXDEIGES
  CAS_TKXDESTEP
  CAS_TKXMesh
  CAS_TKXml
  CAS_TKXmlL
  CAS_TKXmlTObj
  CAS_TKXmlXCAF
  CAS_TKXSBase
)

IF(CAS_VERSION_STR VERSION_LESS "7.0.1")
  LIST(APPEND _libs CAS_FWOSPlugin)
ELSE()
  LIST(APPEND _libs CAS_TKStd)
ENDIF()

IF(CAS_VERSION_STR VERSION_LESS "6.9.1")
  LIST(APPEND _libs CAS_TKNIS)
ENDIF()

IF(CAS_VERSION_STR VERSION_LESS "7.0")
  LIST(APPEND _libs
    CAS_PTKernel
    CAS_TKPCAF
    CAS_TKPLCAF
    CAS_TKPShape
    CAS_TKShapeSchema
    CAS_TKStdLSchema
    CAS_TKStdSchema
    CAS_TKXCAFSchema
    )
ENDIF()


INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CAS REQUIRED_VARS CAS_INCLUDE_DIRS ${_libs})

IF(CAS_FOUND AND NOT CAS_FIND_QUIETLY)
  IF(CAS_VERSION_DEVELOPMENT)
    MESSAGE(STATUS "Found OpenCascade version: ${CAS_VERSION_STR} (development)")
  ELSE(CAS_VERSION_DEVELOPMENT)
    MESSAGE(STATUS "Found OpenCascade version: ${CAS_VERSION_STR}")
  ENDIF(CAS_VERSION_DEVELOPMENT)
ENDIF()
