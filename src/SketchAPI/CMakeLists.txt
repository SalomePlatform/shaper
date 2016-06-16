## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)

SET(PROJECT_HEADERS
  SketchAPI.h
  SketchAPI_Arc.h
  SketchAPI_Circle.h
  SketchAPI_Line.h
  SketchAPI_Mirror.h
  SketchAPI_Sketch.h
  SketchAPI_SketchEntity.h
  SketchAPI_Point.h
  SketchAPI_Translation.h
)

SET(PROJECT_SOURCES
  SketchAPI_Arc.cpp
  SketchAPI_Circle.cpp
  SketchAPI_Line.cpp
  SketchAPI_Mirror.cpp
  SketchAPI_Sketch.cpp
  SketchAPI_SketchEntity.cpp
  SketchAPI_Point.cpp
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
)

# Plugin headers dependency
INCLUDE_DIRECTORIES(
  # TODO(spo): modify ConstructionPlugin headers to remove dependency on GeomAPI headers
  ${PROJECT_SOURCE_DIR}/src/Config
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  ${PROJECT_SOURCE_DIR}/src/SketchPlugin
)

#TODO(spo): is ${CAS_DEFINITIONS} necessary?
ADD_DEFINITIONS(-DSKETCHAPI_EXPORTS ${CAS_DEFINITIONS})
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

SWIG_ADD_MODULE(SketchAPI python SketchAPI.i ${PROJECT_HEADERS})
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
