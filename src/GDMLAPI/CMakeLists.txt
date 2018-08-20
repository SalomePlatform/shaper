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
  GDMLAPI.h
  GDMLAPI_ConeSegment.h
  GDMLAPI_Ellipsoid.h
)

SET(PROJECT_SOURCES
  GDMLAPI_ConeSegment.cpp
  GDMLAPI_Ellipsoid.cpp
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
  ${PROJECT_SOURCE_DIR}/src/GDMLPlugin
)

ADD_DEFINITIONS(-DGDMLAPI_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(GDMLAPI SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(GDMLAPI ${PROJECT_LIBRARIES})

# SWIG wrapper

INCLUDE(PythonAPI)

SET_SOURCE_FILES_PROPERTIES(GDMLAPI.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(GDMLAPI.i PROPERTIES SWIG_DEFINITIONS "-shadow")

SET(SWIG_LINK_LIBRARIES
  GDMLAPI
  ModelHighAPI
  ModelAPI
  ${PYTHON_LIBRARIES}
)

SET(SWIG_MODULE_GDMLAPI_EXTRA_DEPS ${SWIG_MODULE_GDMLAPI_EXTRA_DEPS}
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI/ModelHighAPI.i
  doxyhelp.i
  ${PROJECT_HEADERS}
)

IF(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  SWIG_ADD_LIBRARY(GDMLAPI LANGUAGE python SOURCES GDMLAPI.i ${PROJECT_HEADERS})
ELSE()
  SWIG_ADD_MODULE(GDMLAPI python GDMLAPI.i ${PROJECT_HEADERS})
ENDIF()
SWIG_LINK_LIBRARIES(GDMLAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_GDMLAPI PROPERTIES DEBUG_OUTPUT_NAME _GDMLAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS _GDMLAPI DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(TARGETS GDMLAPI DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/GDMLAPI.py DESTINATION ${SHAPER_INSTALL_SWIG})
