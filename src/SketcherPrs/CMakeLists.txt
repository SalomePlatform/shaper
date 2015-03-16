## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

SET(PROJECT_HEADERS
    SketcherPrs.h
    SketcherPrs_Coincident.h
    SketcherPrs_Factory.h
    SketcherPrs_Parallel.h
    SketcherPrs_Tools.h
    SketcherPrs_Perpendicular.h
    SketcherPrs_SymbolPrs.h
	SketcherPrs_PositionMgr.h
	SketcherPrs_Rigid.h
)

SET(PROJECT_SOURCES
    SketcherPrs_Coincident.cpp
    SketcherPrs_Factory.cpp
    SketcherPrs_Parallel.cpp
    SketcherPrs_Tools.cpp
    SketcherPrs_Perpendicular.cpp
    SketcherPrs_SymbolPrs.cpp
	SketcherPrs_PositionMgr.cpp
	SketcherPrs_Rigid.cpp
)

SET(PROJECT_LIBRARIES
    Config
    ModelAPI
    GeomAPI
    ${CAS_KERNEL} 
    ${CAS_MODELER} 
    ${CAS_VIEWER}
    ${CAS_SHAPE}
    ${CAS_TKTopAlgo}
)

SET(PROJECT_PICTURES
    icons/parallel.png
    icons/perpendicular.png
	icons/anchor.png
)

ADD_DEFINITIONS(-DCONSTRAINTS_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(SketcherPrs SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/Config
  ${PROJECT_SOURCE_DIR}/src/ModelAPI
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
  ${PROJECT_SOURCE_DIR}/src/SketchPlugin
  ${CAS_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(SketcherPrs ${PROJECT_LIBRARIES})

INSTALL(TARGETS SketcherPrs DESTINATION bin)
INSTALL(FILES ${PROJECT_PICTURES} DESTINATION resources)
