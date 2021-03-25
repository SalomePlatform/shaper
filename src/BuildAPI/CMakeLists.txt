# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
  BuildAPI.h
  BuildAPI_Compound.h
  BuildAPI_CompSolid.h
  BuildAPI_Edge.h
  BuildAPI_Face.h
  BuildAPI_Filling.h
  BuildAPI_Shell.h
  BuildAPI_Solid.h
  BuildAPI_SubShapes.h
  BuildAPI_Vertex.h
  BuildAPI_Wire.h
  BuildAPI_Polyline.h
  BuildAPI_Interpolation.h
)

SET(PROJECT_SOURCES
  BuildAPI_Compound.cpp
  BuildAPI_CompSolid.cpp
  BuildAPI_Edge.cpp
  BuildAPI_Face.cpp
  BuildAPI_Filling.cpp
  BuildAPI_Shell.cpp
  BuildAPI_Solid.cpp
  BuildAPI_SubShapes.cpp
  BuildAPI_Vertex.cpp
  BuildAPI_Wire.cpp
  BuildAPI_Polyline.cpp
  BuildAPI_Interpolation.cpp
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
  ${PROJECT_SOURCE_DIR}/src/BuildPlugin
)

#TODO(spo): is ${OpenCASCADE_DEFINITIONS} necessary?
ADD_DEFINITIONS(-DBUILDAPI_EXPORTS ${OpenCASCADE_DEFINITIONS})
ADD_LIBRARY(BuildAPI SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(BuildAPI ${PROJECT_LIBRARIES})

# SWIG wrapper

INCLUDE(PythonAPI)

SET_SOURCE_FILES_PROPERTIES(BuildAPI.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(BuildAPI.i PROPERTIES SWIG_DEFINITIONS "-shadow")

#TODO(spo): is ModelAPI necessary or it could be received by INTERFACE_ (may require modern CMake)?
SET(SWIG_LINK_LIBRARIES
  BuildAPI
  ModelHighAPI
  ModelAPI
  ${PYTHON_LIBRARIES}
)
SET(CMAKE_SWIG_FLAGS -threads -w325,321,302,362,322,383,403)

SET(SWIG_MODULE_BuildAPI_EXTRA_DEPS ${SWIG_MODULE_BuildAPI_EXTRA_DEPS}
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI/ModelHighAPI.i
  doxyhelp.i
  ${PROJECT_HEADERS}
)

IF(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  SWIG_ADD_LIBRARY(BuildAPI LANGUAGE python SOURCES BuildAPI.i ${PROJECT_HEADERS})
ELSE()
  SWIG_ADD_MODULE(BuildAPI python BuildAPI.i ${PROJECT_HEADERS})
ENDIF()
SWIG_LINK_LIBRARIES(BuildAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_BuildAPI PROPERTIES DEBUG_OUTPUT_NAME _BuildAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS _BuildAPI DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(TARGETS BuildAPI DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/BuildAPI.py DESTINATION ${SHAPER_INSTALL_SWIG})

# Tests
INCLUDE(UnitTest)

ADD_UNIT_TESTS(
)

# ADD_SUBDIRECTORY (Test)
