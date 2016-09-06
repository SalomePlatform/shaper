## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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
)

SET(PROJECT_LIBRARIES
  Config
  Events
  GeomAPI
  GeomDataAPI
  GeomAlgoAPI
  ModelAPI
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
  ${PROJECT_SOURCE_DIR}/src/PartSetPlugin
  ${CAS_INCLUDE_DIRS}
)

set(SWIG_MODULE_ModelHighAPI_EXTRA_DEPS
  doxyhelp.i
  ${PROJECT_HEADERS}
)

SWIG_ADD_MODULE(ModelHighAPI python ModelHighAPI.i ${PROJECT_HEADERS})
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
)
