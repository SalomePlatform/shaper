# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

IF(NOT DEFINED ENV{FREETYPE_DIR})
    SET(ENV{FREETYPE_DIR} $ENV{FREETYPE_ROOT_DIR})
ENDIF()
FIND_PACKAGE(Freetype REQUIRED)

SET(PROJECT_HEADERS
    SketcherPrs.h
    SketcherPrs_Coincident.h
    SketcherPrs_Collinear.h
    SketcherPrs_DimensionStyle.h
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
    SketcherPrs_Offset.h
)

SET(PROJECT_SOURCES
    SketcherPrs_Coincident.cpp
    SketcherPrs_Collinear.cpp
    SketcherPrs_DimensionStyle.cpp
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
    SketcherPrs_Offset.cpp
)

SET(PROJECT_LIBRARIES
    Config
    ModelAPI
    ModelGeomAlgo
    GeomAPI
    GeomDataAPI
    Events
    Locale
    ${OpenCASCADE_FoundationClasses_LIBRARIES}
    ${OpenCASCADE_ModelingAlgorithms_LIBRARIES}
    ${OpenCASCADE_Visualization_LIBRARIES}
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
    icons/offset.png
)

ADD_DEFINITIONS(-DSKETCHERPRS_EXPORTS ${OpenCASCADE_DEFINITIONS} -D_CRT_SECURE_NO_WARNINGS)
ADD_LIBRARY(SketcherPrs SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/Config
  ${PROJECT_SOURCE_DIR}/src/Events
  ${PROJECT_SOURCE_DIR}/src/ModelAPI
  ${PROJECT_SOURCE_DIR}/src/ModelGeomAlgo
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
  ${PROJECT_SOURCE_DIR}/src/Locale
  ${PROJECT_SOURCE_DIR}/src/SketchPlugin
  ${OpenCASCADE_INCLUDE_DIR}
  ${FREETYPE_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(SketcherPrs ${PROJECT_LIBRARIES})

INSTALL(TARGETS SketcherPrs DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${PROJECT_PICTURES} DESTINATION ${SHAPER_INSTALL_RESOURCES})
