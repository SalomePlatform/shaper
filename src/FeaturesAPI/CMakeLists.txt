# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

INCLUDE(Common)

SET(PROJECT_HEADERS
  FeaturesAPI.h
  FeaturesAPI_BooleanCut.h
  FeaturesAPI_BooleanFuse.h
  FeaturesAPI_BooleanCommon.h
  FeaturesAPI_BooleanSmash.h
  FeaturesAPI_BooleanFill.h
  FeaturesAPI_Chamfer.h
  FeaturesAPI_Extrusion.h
  FeaturesAPI_ExtrusionBoolean.h
  FeaturesAPI_Fillet.h
  FeaturesAPI_GlueFaces.h
  FeaturesAPI_Intersection.h
  FeaturesAPI_Measurement.h
  FeaturesAPI_NormalToFace.h
  FeaturesAPI_MultiRotation.h
  FeaturesAPI_MultiTranslation.h
  FeaturesAPI_Partition.h
  FeaturesAPI_Pipe.h
  FeaturesAPI_Loft.h
  FeaturesAPI_Placement.h
  FeaturesAPI_PointCloudOnFace.h
  FeaturesAPI_Recover.h
  FeaturesAPI_RemoveResults.h
  FeaturesAPI_RemoveSubShapes.h
  FeaturesAPI_Revolution.h
  FeaturesAPI_RevolutionBoolean.h
  FeaturesAPI_Rotation.h
  FeaturesAPI_Scale.h
  FeaturesAPI_Sewing.h
  FeaturesAPI_Symmetry.h
  FeaturesAPI_Translation.h
  FeaturesAPI_Union.h
  FeaturesAPI_FusionFaces.h
  FeaturesAPI_Copy.h
  FeaturesAPI_ImportResult.h
  FeaturesAPI_Defeaturing.h
  FeaturesAPI_PointCoordinates.h
  FeaturesAPI_GeometryCalculation.h
  FeaturesAPI_BoundingBox.h
)

SET(PROJECT_SOURCES
  FeaturesAPI_BooleanCut.cpp
  FeaturesAPI_BooleanFuse.cpp
  FeaturesAPI_BooleanCommon.cpp
  FeaturesAPI_BooleanSmash.cpp
  FeaturesAPI_BooleanFill.cpp
  FeaturesAPI_Chamfer.cpp
  FeaturesAPI_Extrusion.cpp
  FeaturesAPI_ExtrusionBoolean.cpp
  FeaturesAPI_Fillet.cpp
  FeaturesAPI_GlueFaces.cpp
  FeaturesAPI_Intersection.cpp
  FeaturesAPI_Measurement.cpp
  FeaturesAPI_NormalToFace.cpp
  FeaturesAPI_MultiRotation.cpp
  FeaturesAPI_MultiTranslation.cpp
  FeaturesAPI_Partition.cpp
  FeaturesAPI_Pipe.cpp
  FeaturesAPI_Loft.cpp
  FeaturesAPI_Placement.cpp
  FeaturesAPI_PointCloudOnFace.cpp
  FeaturesAPI_Recover.cpp
  FeaturesAPI_RemoveResults.cpp
  FeaturesAPI_RemoveSubShapes.cpp
  FeaturesAPI_Revolution.cpp
  FeaturesAPI_RevolutionBoolean.cpp
  FeaturesAPI_Rotation.cpp
  FeaturesAPI_Scale.cpp
  FeaturesAPI_Sewing.cpp
  FeaturesAPI_Symmetry.cpp
  FeaturesAPI_Translation.cpp
  FeaturesAPI_Union.cpp
  FeaturesAPI_FusionFaces.cpp
  FeaturesAPI_Copy.cpp
  FeaturesAPI_ImportResult.cpp
  FeaturesAPI_Defeaturing.cpp
  FeaturesAPI_PointCoordinates.cpp
  FeaturesAPI_GeometryCalculation.cpp
  FeaturesAPI_BoundingBox.cpp
)

SET(PROJECT_LIBRARIES
  ModelAPI
  ModelHighAPI
)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/Events
  ${PROJECT_SOURCE_DIR}/src/ModelAPI
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI
)

# Plugin headers dependency
INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
  ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
  ${PROJECT_SOURCE_DIR}/src/FeaturesPlugin
)

ADD_DEFINITIONS(-DFEATURESAPI_EXPORTS)
ADD_LIBRARY(FeaturesAPI SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(FeaturesAPI ${PROJECT_LIBRARIES})

# SWIG wrapper

INCLUDE(PythonAPI)

SET_SOURCE_FILES_PROPERTIES(FeaturesAPI.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(FeaturesAPI.i PROPERTIES SWIG_DEFINITIONS "-shadow")

SET(SWIG_LINK_LIBRARIES
  FeaturesAPI
  ModelHighAPI
  ModelAPI
  ${PYTHON_LIBRARIES}
)
SET(CMAKE_SWIG_FLAGS -threads -w325,321,302,362,322,383,403)

SET(SWIG_MODULE_FeaturesAPI_EXTRA_DEPS ${SWIG_MODULE_FeaturesAPI_EXTRA_DEPS}
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI/ModelHighAPI.i
  doxyhelp.i
  ${PROJECT_HEADERS}
)

IF(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  SWIG_ADD_LIBRARY(FeaturesAPI LANGUAGE python SOURCES FeaturesAPI.i ${PROJECT_HEADERS})
ELSE()
  SWIG_ADD_MODULE(FeaturesAPI python FeaturesAPI.i ${PROJECT_HEADERS})
ENDIF()
SWIG_LINK_LIBRARIES(FeaturesAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_FeaturesAPI PROPERTIES DEBUG_OUTPUT_NAME _FeaturesAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS _FeaturesAPI DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(TARGETS FeaturesAPI DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/FeaturesAPI.py DESTINATION ${SHAPER_INSTALL_SWIG})
