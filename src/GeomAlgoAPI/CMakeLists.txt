## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

FIND_PACKAGE(SWIG REQUIRED)

INCLUDE(${SWIG_USE_FILE})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(PROJECT_HEADERS
    GeomAlgoAPI.h
    GeomAlgoAPI_CompoundBuilder.h
    GeomAlgoAPI_FaceBuilder.h
    GeomAlgoAPI_EdgeBuilder.h
    GeomAlgoAPI_PointBuilder.h
    GeomAlgoAPI_SketchBuilder.h
    GeomAlgoAPI_Extrusion.h
    GeomAlgoAPI_Prism.h
    GeomAlgoAPI_Boolean.h
    GeomAlgoAPI_MakeShape.h
    GeomAlgoAPI_ShapeProps.h
    GeomAlgoAPI_DFLoader.h
    GeomAlgoAPI_Placement.h
    GeomAlgoAPI_BREPImport.h
    GeomAlgoAPI_STEPImport.h
    GeomAlgoAPI_IGESImport.h
    GeomAlgoAPI_BREPExport.h
    GeomAlgoAPI_STEPExport.h
    GeomAlgoAPI_IGESExport.h
)

SET(PROJECT_SOURCES
    GeomAlgoAPI_CompoundBuilder.cpp
    GeomAlgoAPI_FaceBuilder.cpp
    GeomAlgoAPI_EdgeBuilder.cpp
    GeomAlgoAPI_PointBuilder.cpp
    GeomAlgoAPI_SketchBuilder.cpp
    GeomAlgoAPI_Extrusion.cpp
    GeomAlgoAPI_Prism.cpp
    GeomAlgoAPI_Boolean.cpp
    GeomAlgoAPI_MakeShape.cpp
    GeomAlgoAPI_ShapeProps.cpp
    GeomAlgoAPI_DFLoader.cpp
    GeomAlgoAPI_Placement.cpp
    GeomAlgoAPI_BREPImport.cpp
    GeomAlgoAPI_STEPImport.cpp
    GeomAlgoAPI_IGESImport.cpp
    GeomAlgoAPI_BREPExport.cpp
    GeomAlgoAPI_STEPExport.cpp
    GeomAlgoAPI_IGESExport.cpp
)

SET(PROJECT_LIBRARIES
    GeomAPI
    ModelAPI
    ${CAS_OCAF}
    ${CAS_SHAPE}
    ${CAS_TKBO} 
    ${CAS_TKBool} 
    ${CAS_TKBRep}
    ${CAS_TKCAF}
    ${CAS_TKCAF}
    ${CAS_TKLCAF}
    ${CAS_TKPrim}
    ${CAS_TKSTEP}
    ${CAS_TKSTEPBase}
    ${CAS_TKIGES}
    ${CAS_TKTopAlgo}
    ${CAS_TKXSBase} 
)

ADD_DEFINITIONS(-DGEOMALGOAPI_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(GeomAlgoAPI SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})

SET(CMAKE_SWIG_FLAGS "-Wall")

SET_SOURCE_FILES_PROPERTIES(GeomAlgoAPI.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(GeomAlgoAPI.i PROPERTIES SWIG_DEFINITIONS "-shadow")

INCLUDE_DIRECTORIES(
  ../GeomAPI
  ../ModelAPI
  ${CAS_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(GeomAlgoAPI ${PROJECT_LIBRARIES})

SET(SWIG_SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/GeomAlgoAPI.py
)

SET(SWIG_LINK_LIBRARIES
  GeomAPI
  GeomAlgoAPI
  ${PYTHON_LIBRARIES}
)

SWIG_ADD_MODULE(GeomAlgoAPI python GeomAlgoAPI.i ${PROJECT_HEADERS})
SWIG_LINK_LIBRARIES(GeomAlgoAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_GeomAlgoAPI PROPERTIES DEBUG_OUTPUT_NAME _GeomAlgoAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS _GeomAlgoAPI DESTINATION swig)
INSTALL(TARGETS GeomAlgoAPI DESTINATION bin)
INSTALL(FILES ${SWIG_SCRIPTS} DESTINATION swig)
