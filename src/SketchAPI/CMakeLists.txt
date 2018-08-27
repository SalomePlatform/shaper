## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

INCLUDE(Common)

SET(PROJECT_HEADERS
  SketchAPI.h
  SketchAPI_Arc.h
  SketchAPI_Circle.h
  SketchAPI_Constraint.h
  SketchAPI_IntersectionPoint.h
  SketchAPI_Line.h
  SketchAPI_MacroArc.h
  SketchAPI_MacroCircle.h
  SketchAPI_Mirror.h
  SketchAPI_Sketch.h
  SketchAPI_SketchEntity.h
  SketchAPI_Point.h
  SketchAPI_Projection.h
  SketchAPI_Rectangle.h
  SketchAPI_Rotation.h
  SketchAPI_Translation.h
)

SET(PROJECT_SOURCES
  SketchAPI_Arc.cpp
  SketchAPI_Circle.cpp
  SketchAPI_Constraint.cpp
  SketchAPI_IntersectionPoint.cpp
  SketchAPI_Line.cpp
  SketchAPI_MacroArc.cpp
  SketchAPI_MacroCircle.cpp
  SketchAPI_Mirror.cpp
  SketchAPI_Sketch.cpp
  SketchAPI_SketchEntity.cpp
  SketchAPI_Point.cpp
  SketchAPI_Projection.cpp
  SketchAPI_Rectangle.cpp
  SketchAPI_Rotation.cpp
  SketchAPI_Translation.cpp
)

SET(PROJECT_LIBRARIES
  ModelAPI
  ModelHighAPI
)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/Events
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  ${PROJECT_SOURCE_DIR}/src/ModelAPI
  ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI
  ${PROJECT_SOURCE_DIR}/src/SketcherPrs
)

# Plugin headers dependency
INCLUDE_DIRECTORIES(
  # TODO(spo): modify ConstructionPlugin headers to remove dependency on GeomAPI headers
  ${PROJECT_SOURCE_DIR}/src/Config
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  ${PROJECT_SOURCE_DIR}/src/SketchPlugin
)

#TODO(spo): is ${OpenCASCADE_DEFINITIONS} necessary?
ADD_DEFINITIONS(-DSKETCHAPI_EXPORTS ${OpenCASCADE_DEFINITIONS})
ADD_LIBRARY(SketchAPI SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(SketchAPI ${PROJECT_LIBRARIES})

# SWIG wrapper

INCLUDE(PythonAPI)

SET_SOURCE_FILES_PROPERTIES(SketchAPI.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(SketchAPI.i PROPERTIES SWIG_DEFINITIONS "-shadow")

#TODO(spo): is ModelAPI necessary or it could be received by INTERFACE_ (may require modern CMake)?
SET(SWIG_LINK_LIBRARIES
  SketchAPI
  ModelHighAPI
  ModelAPI
  ${PYTHON_LIBRARIES}
)

SET(SWIG_MODULE_SketchAPI_EXTRA_DEPS ${SWIG_MODULE_SketchAPI_EXTRA_DEPS}
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI/ModelHighAPI.i
  doxyhelp.i
  ${PROJECT_HEADERS}
)

IF(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  SWIG_ADD_LIBRARY(SketchAPI LANGUAGE python SOURCES SketchAPI.i ${PROJECT_HEADERS})
ELSE()
  SWIG_ADD_MODULE(SketchAPI python SketchAPI.i ${PROJECT_HEADERS})
ENDIF()
SWIG_LINK_LIBRARIES(SketchAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_SketchAPI PROPERTIES DEBUG_OUTPUT_NAME _SketchAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS _SketchAPI DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(TARGETS SketchAPI DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/SketchAPI.py DESTINATION ${SHAPER_INSTALL_SWIG})

# Tests

INCLUDE(UnitTest)

ADD_UNIT_TESTS(
  TestSketch.py
)

# ADD_SUBDIRECTORY (Test)
