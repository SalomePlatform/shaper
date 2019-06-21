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

INCLUDE(Common)

SET(PROJECT_HEADERS
  FiltersAPI.h
  FiltersAPI_Feature.h
  FiltersAPI_Filter.h
  FiltersAPI_Selection.h
)

SET(PROJECT_SOURCES
  FiltersAPI_Feature.cpp
  FiltersAPI_Filter.cpp
  FiltersAPI_Selection.cpp
)

SET(PROJECT_LIBRARIES
  ModelAPI
  ModelHighAPI
)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/Events
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
  ${PROJECT_SOURCE_DIR}/src/ModelAPI
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI
)

# Plugin headers dependency
INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/FiltersPlugin
)

ADD_DEFINITIONS(-DFILTERSAPI_EXPORTS)
ADD_LIBRARY(FiltersAPI SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(FiltersAPI ${PROJECT_LIBRARIES})

# SWIG wrapper

INCLUDE(PythonAPI)

SET_SOURCE_FILES_PROPERTIES(FiltersAPI.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(FiltersAPI.i PROPERTIES SWIG_DEFINITIONS "-shadow")

SET(SWIG_LINK_LIBRARIES
  FiltersAPI
  ModelHighAPI
  ModelAPI
  ${PYTHON_LIBRARIES}
)
SET(CMAKE_SWIG_FLAGS -threads -w325,321,302,362,322,383,403)

SET(SWIG_MODULE_FiltersAPI_EXTRA_DEPS ${SWIG_MODULE_FiltersAPI_EXTRA_DEPS}
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI/ModelHighAPI.i
  doxyhelp.i
  ${PROJECT_HEADERS}
)

IF(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  SWIG_ADD_LIBRARY(FiltersAPI LANGUAGE python SOURCES FiltersAPI.i ${PROJECT_HEADERS})
ELSE()
  SWIG_ADD_MODULE(FiltersAPI python FiltersAPI.i ${PROJECT_HEADERS})
ENDIF()
SWIG_LINK_LIBRARIES(FiltersAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_FiltersAPI PROPERTIES DEBUG_OUTPUT_NAME _FiltersAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS _FiltersAPI DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(TARGETS FiltersAPI DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/FiltersAPI.py DESTINATION ${SHAPER_INSTALL_SWIG})
