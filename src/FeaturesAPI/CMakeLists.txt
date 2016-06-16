## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)

SET(PROJECT_HEADERS
  FeaturesAPI.h
  FeaturesAPI_Boolean.h
  FeaturesAPI_Extrusion.h
  FeaturesAPI_ExtrusionBoolean.h
  FeaturesAPI_Group.h
  FeaturesAPI_Intersection.h
  FeaturesAPI_Partition.h
  FeaturesAPI_Pipe.h
  FeaturesAPI_Placement.h
  FeaturesAPI_Revolution.h
  FeaturesAPI_RevolutionBoolean.h
  FeaturesAPI_Rotation.h
  FeaturesAPI_Translation.h
)

SET(PROJECT_SOURCES
  FeaturesAPI_Boolean.cpp
  FeaturesAPI_Extrusion.cpp
  FeaturesAPI_ExtrusionBoolean.cpp
  FeaturesAPI_Group.cpp
  FeaturesAPI_Intersection.cpp
  FeaturesAPI_Partition.cpp
  FeaturesAPI_Pipe.cpp
  FeaturesAPI_Placement.cpp
  FeaturesAPI_Rotation.cpp
  FeaturesAPI_Revolution.cpp
  FeaturesAPI_RevolutionBoolean.cpp
  FeaturesAPI_Translation.cpp
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

#TODO(spo): is ${CAS_DEFINITIONS} necessary?
ADD_DEFINITIONS(-DFEATURESAPI_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(FeaturesAPI SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(FeaturesAPI ${PROJECT_LIBRARIES})

# SWIG wrapper

INCLUDE(PythonAPI)

SET_SOURCE_FILES_PROPERTIES(FeaturesAPI.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(FeaturesAPI.i PROPERTIES SWIG_DEFINITIONS "-shadow")

#TODO(spo): is ModelAPI necessary or it could be received by INTERFACE_ (may require modern CMake)?
SET(SWIG_LINK_LIBRARIES
  FeaturesAPI
  ModelHighAPI
  ModelAPI
  ${PYTHON_LIBRARIES}
)

SET(SWIG_MODULE_FeaturesAPI_EXTRA_DEPS ${SWIG_MODULE_FeaturesAPI_EXTRA_DEPS}
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI/ModelHighAPI.i
  doxyhelp.i
  ${PROJECT_HEADERS}
)

SWIG_ADD_MODULE(FeaturesAPI python FeaturesAPI.i ${PROJECT_HEADERS})
SWIG_LINK_LIBRARIES(FeaturesAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_FeaturesAPI PROPERTIES DEBUG_OUTPUT_NAME _FeaturesAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS _FeaturesAPI DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(TARGETS FeaturesAPI DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/FeaturesAPI.py DESTINATION ${SHAPER_INSTALL_SWIG})

# Tests
INCLUDE(UnitTest)

ADD_UNIT_TESTS(
)

# ADD_SUBDIRECTORY (Test)
