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

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(PROJECT_HEADERS
    GeomAlgoImpl.h
    GEOMAlgo_Algo.hxx
    GEOMAlgo_AlgoTools.hxx
    GEOMAlgo_BndSphere.hxx
    GEOMAlgo_BndSphereTree.hxx
    GEOMAlgo_BuilderShape.hxx
    GEOMAlgo_CoupleOfShapes.hxx
    GEOMAlgo_GlueDetector.hxx
    GEOMAlgo_Gluer2.hxx
    GEOMAlgo_GluerAlgo.hxx
    GEOMAlgo_IndexedDataMapOfIntegerShape.hxx
    GEOMAlgo_IndexedDataMapOfPassKeyShapeListOfShape.hxx
    GEOMAlgo_IndexedDataMapOfShapeBndSphere.hxx
    GEOMAlgo_IndexedDataMapOfShapeIndexedMapOfShape.hxx
    GEOMAlgo_ListIteratorOfListOfCoupleOfShapes.hxx
    GEOMAlgo_ListOfCoupleOfShapes.hxx
    GEOMAlgo_PassKeyShape.hxx
    GEOMAlgo_PassKeyShapeMapHasher.hxx
    GEOMAlgo_Splitter.hxx
    GEOMImpl_Fillet1d.hxx
)

SET(PROJECT_SOURCES
    GEOMAlgo_Algo.cxx
    GEOMAlgo_AlgoTools.cxx
    GEOMAlgo_AlgoTools_1.cxx
    GEOMAlgo_BndSphere.cxx
    GEOMAlgo_BndSphereTree.cxx
    GEOMAlgo_BuilderShape.cxx
    GEOMAlgo_CoupleOfShapes.cxx
    GEOMAlgo_GlueDetector.cxx
    GEOMAlgo_Gluer2.cxx
    GEOMAlgo_Gluer2_1.cxx
    GEOMAlgo_Gluer2_2.cxx
    GEOMAlgo_Gluer2_3.cxx
    GEOMAlgo_GluerAlgo.cxx
    GEOMAlgo_PassKeyShape.cxx
    GEOMAlgo_PassKeyShapeMapHasher.cxx
    GEOMAlgo_Splitter.cxx
    GEOMImpl_Fillet1d.cxx
)

SET(PROJECT_LIBRARIES
  ${OpenCASCADE_FoundationClasses_LIBRARIES}
  ${OpenCASCADE_ModelingAlgorithms_LIBRARIES}
)

ADD_DEFINITIONS(-DGEOMALGOIMPL_EXPORTS ${OpenCASCADE_DEFINITIONS})
ADD_LIBRARY(GeomAlgoImpl SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})

INCLUDE_DIRECTORIES(
  ${OpenCASCADE_INCLUDE_DIR}
)

TARGET_LINK_LIBRARIES(GeomAlgoImpl ${PROJECT_LIBRARIES})

INSTALL(TARGETS GeomAlgoImpl DESTINATION ${SHAPER_INSTALL_BIN})
