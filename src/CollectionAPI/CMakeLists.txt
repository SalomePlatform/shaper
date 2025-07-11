# Copyright (C) 2014-2025  CEA, EDF
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
  CollectionAPI.h
  CollectionAPI_Group.h
  CollectionAPI_GroupAddition.h
  CollectionAPI_GroupShape.h
  CollectionAPI_GroupIntersection.h
  CollectionAPI_GroupSubstraction.h
  CollectionAPI_Field.h
)

SET(PROJECT_SOURCES
  CollectionAPI_Group.cpp
  CollectionAPI_GroupAddition.cpp
  CollectionAPI_GroupShape.cpp
  CollectionAPI_GroupIntersection.cpp
  CollectionAPI_GroupSubstraction.cpp
  CollectionAPI_Field.cpp
)

SET(PROJECT_LIBRARIES
  ModelAPI
  ModelHighAPI
  Locale
  ${PyInterp}
  ${PYTHON_LIBRARIES}
)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/Events
  ${PROJECT_SOURCE_DIR}/src/ModelAPI
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI
  ${PROJECT_SOURCE_DIR}/src/Locale
  ${SUIT_INCLUDE}
)

# Plugin headers dependency
INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
  ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
  ${PROJECT_SOURCE_DIR}/src/CollectionPlugin
)

ADD_DEFINITIONS(-DCOLLECTIONAPI_EXPORTS)
ADD_LIBRARY(CollectionAPI SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(CollectionAPI ${PROJECT_LIBRARIES})

# SWIG wrapper

INCLUDE(PythonAPI)

SET(CMAKE_SWIG_FLAGS -threads -w325,321,302,362,322,383,403,509)

SET_SOURCE_FILES_PROPERTIES(CollectionAPI.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(CollectionAPI.i PROPERTIES SWIG_DEFINITIONS "-shadow")

SET(SWIG_LINK_LIBRARIES
  CollectionAPI
  ModelHighAPI
  ModelAPI
  ${PyInterp}
  ${PYTHON_LIBRARIES}
)

SET(SWIG_MODULE_CollectionAPI_EXTRA_DEPS ${SWIG_MODULE_CollectionAPI_EXTRA_DEPS}
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI/ModelHighAPI.i
  doxyhelp.i
  ${PROJECT_HEADERS}
)

IF(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  SWIG_ADD_LIBRARY(CollectionAPI LANGUAGE python SOURCES CollectionAPI.i ${PROJECT_HEADERS})
ELSE()
  SWIG_ADD_MODULE(CollectionAPI python CollectionAPI.i ${PROJECT_HEADERS})
ENDIF()
SWIG_LINK_LIBRARIES(CollectionAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_CollectionAPI PROPERTIES DEBUG_OUTPUT_NAME _CollectionAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS _CollectionAPI DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(TARGETS CollectionAPI DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/CollectionAPI.py DESTINATION ${SHAPER_INSTALL_SWIG})
