# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    ModelGeomAlgo.h
    ModelGeomAlgo_Point2D.h
    ModelGeomAlgo_Shape.h
)

SET(PROJECT_SOURCES
    ModelGeomAlgo_Point2D.cpp
    ModelGeomAlgo_Shape.cpp
)

SET(PROJECT_LIBRARIES
    Config
    GeomAPI
    GeomDataAPI
    GeomAlgoAPI
    ModelAPI
)

SET(CMAKE_SWIG_FLAGS -threads -Wall)
ADD_DEFINITIONS(-DMODELGEOMALGO_EXPORTS)

ADD_LIBRARY(ModelGeomAlgo SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
SET_TARGET_PROPERTIES(ModelGeomAlgo PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(ModelGeomAlgo ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
)


SET_SOURCE_FILES_PROPERTIES(ModelGeomAlgo.i PROPERTIES CPLUSPLUS ON)
# "-includeall" is not needed: it starts to follow the standard inludes (like "string") without success
# SET_SOURCE_FILES_PROPERTIES(ModelGeomAlgo.i PROPERTIES SWIG_FLAGS)
SET_SOURCE_FILES_PROPERTIES(ModelGeomAlgo.i PROPERTIES SWIG_DEFINITIONS "-shadow")
#SET_SOURCE_FILES_PROPERTIES(ModelGeomAlgoPYTHON_wrap.cxx PROPERTIES COMPILE_FLAGS "-D_WIN32")

SET(SWIG_SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/ModelGeomAlgo.py
)

SET(SWIG_LINK_LIBRARIES
  ModelGeomAlgo
  GeomAPI
  ${PYTHON_LIBRARIES}
)

IF(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  SWIG_ADD_LIBRARY(ModelGeomAlgo LANGUAGE python SOURCES ModelGeomAlgo.i ${PROJECT_HEADERS})
ELSE()
  SWIG_ADD_MODULE(ModelGeomAlgo python ModelGeomAlgo.i ${PROJECT_HEADERS})
ENDIF()
SWIG_LINK_LIBRARIES(ModelGeomAlgo ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_ModelGeomAlgo PROPERTIES DEBUG_OUTPUT_NAME _ModelGeomAlgo_d)
ENDIF(WIN32)

INSTALL(TARGETS _ModelGeomAlgo DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(TARGETS ModelGeomAlgo DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${SWIG_SCRIPTS} DESTINATION ${SHAPER_INSTALL_SWIG})

ADD_UNIT_TESTS(TestPoint2D.py)
