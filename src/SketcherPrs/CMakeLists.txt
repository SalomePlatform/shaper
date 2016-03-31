## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

IF(NOT DEFINED ENV{FREETYPE_DIR})
    SET(ENV{FREETYPE_DIR} $ENV{FREETYPE_ROOT_DIR})
ENDIF()
FIND_PACKAGE(Freetype REQUIRED)

SET(PROJECT_HEADERS
    SketcherPrs.h
    SketcherPrs_Coincident.h
    SketcherPrs_Collinear.h
    SketcherPrs_DimensionStyleListener.h
    SketcherPrs_Factory.h
    SketcherPrs_Parallel.h
    SketcherPrs_Tools.h
    SketcherPrs_Perpendicular.h
    SketcherPrs_SymbolPrs.h
    SketcherPrs_PositionMgr.h
    SketcherPrs_Rigid.h
    SketcherPrs_HVDirection.h
    SketcherPrs_Equal.h
    SketcherPrs_Tangent.h
    SketcherPrs_SensitivePoint.h
    SketcherPrs_Radius.h
    SketcherPrs_LengthDimension.h
    SketcherPrs_Middle.h
    SketcherPrs_Mirror.h
    SketcherPrs_Transformation.h
    SketcherPrs_Angle.h
    AIS_AngleDimension.hxx
)

SET(PROJECT_SOURCES
    SketcherPrs_Coincident.cpp
    SketcherPrs_Collinear.cpp
    SketcherPrs_DimensionStyleListener.cpp
    SketcherPrs_Factory.cpp
    SketcherPrs_Parallel.cpp
    SketcherPrs_Tools.cpp
    SketcherPrs_Perpendicular.cpp
    SketcherPrs_SymbolPrs.cpp
    SketcherPrs_PositionMgr.cpp
    SketcherPrs_Rigid.cpp
    SketcherPrs_HVDirection.cpp
    SketcherPrs_Equal.cpp
    SketcherPrs_Tangent.cpp
    SketcherPrs_SensitivePoint.cpp
    SketcherPrs_Radius.cpp
    SketcherPrs_LengthDimension.cpp
    SketcherPrs_Middle.cpp
    SketcherPrs_Mirror.cpp
    SketcherPrs_Transformation.cpp
    SketcherPrs_Angle.cpp
    AIS_AngleDimension.cxx
)

SET(PROJECT_LIBRARIES
    Config
    ModelAPI
    GeomAPI
    GeomDataAPI
    Events
    ${CAS_KERNEL}
    ${CAS_MODELER}
    ${CAS_VIEWER}
    ${CAS_SHAPE}
    ${CAS_TKTopAlgo}
    ${CAS_TKOpenGl}
    ${FREETYPE_LIBRARIES}
)

IF(WIN32)
    SET(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} opengl32)
ELSE()
    SET(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} GL)
ENDIF()


SET(PROJECT_PICTURES
    icons/collinear.png
    icons/parallel.png
    icons/perpendicular.png
    icons/anchor.png
    icons/horisontal.png
    icons/vertical.png
    icons/equal.png
    icons/tangent.png
    icons/middlepoint.png
    icons/mirror.png
    icons/rotate.png
    icons/translate.png
)

ADD_DEFINITIONS(-DSKETCHERPRS_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(SketcherPrs SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/Config
  ${PROJECT_SOURCE_DIR}/src/Events
  ${PROJECT_SOURCE_DIR}/src/ModelAPI
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
  ${PROJECT_SOURCE_DIR}/src/SketchPlugin
  ${CAS_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(SketcherPrs ${PROJECT_LIBRARIES})

INSTALL(TARGETS SketcherPrs DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${PROJECT_PICTURES} DESTINATION ${SHAPER_INSTALL_RESOURCES})
