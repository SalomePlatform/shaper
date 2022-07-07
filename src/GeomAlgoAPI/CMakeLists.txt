# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

FIND_PACKAGE(SWIG REQUIRED)

INCLUDE(${SWIG_USE_FILE})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(PROJECT_HEADERS
    GeomAlgoAPI.h
    GeomAlgoAPI_Tools.h
    GeomAlgoAPI_CompoundBuilder.h
    GeomAlgoAPI_FaceBuilder.h
    GeomAlgoAPI_EdgeBuilder.h
    GeomAlgoAPI_PointBuilder.h
    GeomAlgoAPI_SketchBuilder.h
    GeomAlgoAPI_Prism.h
    GeomAlgoAPI_Revolution.h
    GeomAlgoAPI_Boolean.h
    GeomAlgoAPI_ThroughAll.h
    GeomAlgoAPI_Rotation.h
    GeomAlgoAPI_Translation.h
    GeomAlgoAPI_MakeShape.h
    GeomAlgoAPI_MakeShapeCustom.h
    GeomAlgoAPI_MakeShapeList.h
    GeomAlgoAPI_MakeShapeSet.h
    GeomAlgoAPI_MakeSweep.h
    GeomAlgoAPI_MakeVolume.h
    GeomAlgoAPI_DFLoader.h
    GeomAlgoAPI_Placement.h
    GeomAlgoAPI_BREPImport.h
    GeomAlgoAPI_STEPImport.h
    GeomAlgoAPI_STEPImportXCAF.h
    GeomAlgoAPI_IGESImport.h
    GeomAlgoAPI_BREPExport.h
    GeomAlgoAPI_STEPExport.h
    GeomAlgoAPI_STLExport.h
    GeomAlgoAPI_STLImport.h
    GeomAlgoAPI_ImageImport.h
    GeomAlgoAPI_IGESExport.h
    GeomAlgoAPI_Transform.h
    GeomAlgoAPI_ShapeTools.h
    GeomAlgoAPI_Partition.h
    GeomAlgoAPI_PaveFiller.h
    GeomAlgoAPI_Intersection.h
    GeomAlgoAPI_Pipe.h
    GeomAlgoAPI_WireBuilder.h
    GeomAlgoAPI_Sewing.h
    GeomAlgoAPI_ShapeBuilder.h
    GeomAlgoAPI_ShapeAPI.h
    GeomAlgoAPI_Exception.h
    GeomAlgoAPI_Box.h
    GeomAlgoAPI_Cone.h
    GeomAlgoAPI_Cylinder.h
    GeomAlgoAPI_Sphere.h
    GeomAlgoAPI_Torus.h
    GeomAlgoAPI_XAOExport.h
    GeomAlgoAPI_XAOImport.h
    GeomAlgoAPI_Copy.h
    GeomAlgoAPI_ConeSegment.h
    GeomAlgoAPI_Ellipsoid.h
    GeomAlgoAPI_Symmetry.h
    GeomAlgoAPI_Scale.h
    GeomAlgoAPI_Circ2dBuilder.h
    GeomAlgoAPI_UnifySameDomain.h
    GeomAlgoAPI_Fillet.h
    GeomAlgoAPI_Fillet1D.h
    GeomAlgoAPI_SortListOfShapes.h
    GeomAlgoAPI_Filling.h
    GeomAlgoAPI_CurveBuilder.h
    GeomAlgoAPI_NExplode.h
    GeomAlgoAPI_Offset.h
    GeomAlgoAPI_SolidClassifier.h
    GeomAlgoAPI_MapShapesAndAncestors.h
    GeomAlgoAPI_Projection.h
    GeomAlgoAPI_Chamfer.h
    GeomAlgoAPI_Defeaturing.h
    GeomAlgoAPI_BoundingBox.h
    GeomAlgoAPI_NormalToFace.h
    GeomAlgoAPI_Tube.h
    GeomAlgoAPI_ShapeInfo.h
)

SET(PROJECT_SOURCES
    GeomAlgoAPI_Tools.cpp
    GeomAlgoAPI_CompoundBuilder.cpp
    GeomAlgoAPI_FaceBuilder.cpp
    GeomAlgoAPI_EdgeBuilder.cpp
    GeomAlgoAPI_PointBuilder.cpp
    GeomAlgoAPI_SketchBuilder.cpp
    GeomAlgoAPI_Prism.cpp
    GeomAlgoAPI_Revolution.cpp
    GeomAlgoAPI_Boolean.cpp
    GeomAlgoAPI_ThroughAll.cpp
    GeomAlgoAPI_Rotation.cpp
    GeomAlgoAPI_Translation.cpp
    GeomAlgoAPI_MakeShape.cpp
    GeomAlgoAPI_MakeShapeCustom.cpp
    GeomAlgoAPI_MakeShapeList.cpp
    GeomAlgoAPI_MakeShapeSet.cpp
    GeomAlgoAPI_MakeSweep.cpp
    GeomAlgoAPI_MakeVolume.cpp
    GeomAlgoAPI_DFLoader.cpp
    GeomAlgoAPI_Placement.cpp
    GeomAlgoAPI_BREPImport.cpp
    GeomAlgoAPI_STEPImport.cpp
    GeomAlgoAPI_STEPImportXCAF.cpp
    GeomAlgoAPI_IGESImport.cpp
    GeomAlgoAPI_BREPExport.cpp
    GeomAlgoAPI_STEPExport.cpp
    GeomAlgoAPI_STLExport.cpp
    GeomAlgoAPI_STLImport.cpp
    GeomAlgoAPI_ImageImport.cpp
    GeomAlgoAPI_IGESExport.cpp
    GeomAlgoAPI_Transform.cpp
    GeomAlgoAPI_ShapeTools.cpp
    GeomAlgoAPI_Partition.cpp
    GeomAlgoAPI_PaveFiller.cpp
    GeomAlgoAPI_Intersection.cpp
    GeomAlgoAPI_Pipe.cpp
    GeomAlgoAPI_WireBuilder.cpp
    GeomAlgoAPI_Sewing.cpp
    GeomAlgoAPI_ShapeBuilder.cpp
    GeomAlgoAPI_ShapeAPI.cpp
    GeomAlgoAPI_Exception.cpp
    GeomAlgoAPI_Box.cpp
    GeomAlgoAPI_Cone.cpp
    GeomAlgoAPI_Cylinder.cpp
    GeomAlgoAPI_Sphere.cpp
    GeomAlgoAPI_Torus.cpp
    GeomAlgoAPI_XAOExport.cpp
    GeomAlgoAPI_XAOImport.cpp
    GeomAlgoAPI_Copy.cpp
    GeomAlgoAPI_ConeSegment.cpp
    GeomAlgoAPI_Ellipsoid.cpp
    GeomAlgoAPI_Symmetry.cpp
    GeomAlgoAPI_Scale.cpp
    GeomAlgoAPI_Circ2dBuilder.cpp
    GeomAlgoAPI_UnifySameDomain.cpp
    GeomAlgoAPI_Fillet.cpp
    GeomAlgoAPI_Fillet1D.cpp
    GeomAlgoAPI_SortListOfShapes.cpp
    GeomAlgoAPI_Filling.cpp
    GeomAlgoAPI_CurveBuilder.cpp
    GeomAlgoAPI_NExplode.cpp
    GeomAlgoAPI_Offset.cpp
    GeomAlgoAPI_SolidClassifier.cpp
    GeomAlgoAPI_MapShapesAndAncestors.cpp
    GeomAlgoAPI_Projection.cpp
    GeomAlgoAPI_Chamfer.cpp
    GeomAlgoAPI_Defeaturing.cpp
    GeomAlgoAPI_BoundingBox.cpp
    GeomAlgoAPI_NormalToFace.cpp
    GeomAlgoAPI_Tube.cpp
    GeomAlgoAPI_ShapeInfo.cpp
)

SET(PROJECT_LIBRARIES
    GeomAPI
    GeomAlgoImpl
    ModelAPI
    XAOShaper
    ${OpenCASCADE_DataExchange_LIBRARIES}
    ${OpenCASCADE_ModelingAlgorithms_LIBRARIES}
    ${OpenCASCADE_ApplicationFramework_LIBRARIES}
)

ADD_DEFINITIONS(-DGEOMALGOAPI_EXPORTS ${OpenCASCADE_DEFINITIONS})
ADD_LIBRARY(GeomAlgoAPI SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})

SET(CMAKE_SWIG_FLAGS -w325,321,362,383,302)

SET_SOURCE_FILES_PROPERTIES(GeomAlgoAPI.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(GeomAlgoAPI.i PROPERTIES SWIG_DEFINITIONS "-shadow")

# additional include directories
INCLUDE_DIRECTORIES(
  ../GeomAPI
  ../GeomAlgoImpl
  ../ModelAPI
  ../XAO
  ${PROJECT_SOURCE_DIR}/src/Locale
  ${OpenCASCADE_INCLUDE_DIR}
)

TARGET_LINK_LIBRARIES(GeomAlgoAPI ${PROJECT_LIBRARIES})

SET(SWIG_SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/GeomAlgoAPI.py
)

SET(SWIG_LINK_LIBRARIES
  GeomAPI
  GeomAlgoAPI
  GeomAlgoImpl
  ${PYTHON_LIBRARIES}
)

IF(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  SWIG_ADD_LIBRARY(GeomAlgoAPI LANGUAGE python SOURCES GeomAlgoAPI.i ${PROJECT_HEADERS})
ELSE()
  SWIG_ADD_MODULE(GeomAlgoAPI python GeomAlgoAPI.i ${PROJECT_HEADERS})
ENDIF()
SWIG_LINK_LIBRARIES(GeomAlgoAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_GeomAlgoAPI PROPERTIES DEBUG_OUTPUT_NAME _GeomAlgoAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS _GeomAlgoAPI DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(TARGETS GeomAlgoAPI DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${SWIG_SCRIPTS} DESTINATION ${SHAPER_INSTALL_SWIG})

