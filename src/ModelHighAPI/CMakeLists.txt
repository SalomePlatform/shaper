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
  ModelHighAPI.h
  ModelHighAPI_Double.h
  ModelHighAPI_Dumper.h
  ModelHighAPI_Integer.h
  ModelHighAPI_Interface.h
  ModelHighAPI_Macro.h
  ModelHighAPI_RefAttr.h
  ModelHighAPI_Reference.h
  ModelHighAPI_Selection.h
  ModelHighAPI_Services.h
  ModelHighAPI_Tools.h
  ModelHighAPI_FeatureStore.h
  ModelHighAPI_Folder.h
)

SET(PROJECT_SOURCES
  ModelHighAPI_Double.cpp
  ModelHighAPI_Dumper.cpp
  ModelHighAPI_Integer.cpp
  ModelHighAPI_Interface.cpp
  ModelHighAPI_RefAttr.cpp
  ModelHighAPI_Reference.cpp
  ModelHighAPI_Selection.cpp
  ModelHighAPI_Services.cpp
  ModelHighAPI_Tools.cpp
  ModelHighAPI_FeatureStore.cpp
  ModelHighAPI_Folder.cpp
)

SET(PROJECT_LIBRARIES
  Config
  Events
  GeomAPI
  GeomDataAPI
  GeomAlgoAPI
  ModelAPI
  ModelGeomAlgo
)

ADD_DEFINITIONS(-DMODELHIGHAPI_EXPORTS -DWNT)
ADD_LIBRARY(ModelHighAPI SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
#TODO(spo): is it necessary?
SET_TARGET_PROPERTIES(ModelHighAPI PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(ModelHighAPI ${PROJECT_LIBRARIES})

# SWIG wrapper

INCLUDE(PythonAPI)

SET_SOURCE_FILES_PROPERTIES(ModelHighAPI.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(ModelHighAPI.i PROPERTIES SWIG_DEFINITIONS "-shadow")

SET(SWIG_LINK_LIBRARIES
  ModelHighAPI
  ${PYTHON_LIBRARIES}
)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/Config
  ${PROJECT_SOURCE_DIR}/src/Events
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
  ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
  ${PROJECT_SOURCE_DIR}/src/ModelAPI
  ${PROJECT_SOURCE_DIR}/src/ModelGeomAlgo
  ${PROJECT_SOURCE_DIR}/src/PartSetPlugin
  ${OpenCASCADE_INCLUDE_DIR}
)

set(SWIG_MODULE_ModelHighAPI_EXTRA_DEPS
  doxyhelp.i
  ${PROJECT_HEADERS}
)

IF(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  SWIG_ADD_LIBRARY(ModelHighAPI LANGUAGE python SOURCES ModelHighAPI.i ${PROJECT_HEADERS})
ELSE()
  SWIG_ADD_MODULE(ModelHighAPI python ModelHighAPI.i ${PROJECT_HEADERS})
ENDIF()
SWIG_LINK_LIBRARIES(ModelHighAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_ModelHighAPI PROPERTIES DEBUG_OUTPUT_NAME _ModelHighAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS _ModelHighAPI DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(TARGETS ModelHighAPI DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/ModelHighAPI.py DESTINATION ${SHAPER_INSTALL_SWIG})

# Tests

INCLUDE(UnitTest)

ADD_UNIT_TESTS(
  TestDouble.py
  TestInteger.py
  TestRefAttr.py
  TestReference.py
  TestDeflectionDump.py
  Test2488.py
)
