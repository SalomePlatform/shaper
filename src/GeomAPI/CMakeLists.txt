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

FIND_PACKAGE(SWIG REQUIRED)

INCLUDE(${SWIG_USE_FILE})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    GeomAPI.h
    GeomAPI_Circ.h
    GeomAPI_Circ2d.h
    GeomAPI_Interface.h
    GeomAPI_XY.h
    GeomAPI_XYZ.h
    GeomAPI_Pnt.h
    GeomAPI_Pnt2d.h
    GeomAPI_Lin.h
    GeomAPI_Lin2d.h
    GeomAPI_Dir.h
    GeomAPI_Dir2d.h
    GeomAPI_Pln.h
    GeomAPI_Shape.h
    GeomAPI_ShapeExplorer.h
    GeomAPI_ShapeIterator.h
    GeomAPI_Edge.h
    GeomAPI_Face.h
    GeomAPI_PlanarEdges.h
    GeomAPI_AISObject.h
    GeomAPI_IPresentable.h
    GeomAPI_Curve.h
    GeomAPI_DataMapOfShapeMapOfShapes.h
    GeomAPI_DataMapOfShapeShape.h
    GeomAPI_ICustomPrs.h
    GeomAPI_Vertex.h
    GeomAPI_Ax1.h
    GeomAPI_Ax2.h
    GeomAPI_Ax3.h
    GeomAPI_Trsf.h
    GeomAPI_Angle2d.h
    GeomAPI_Angle.h
    GeomAPI_Wire.h
    GeomAPI_Ellipse.h
    GeomAPI_Ellipse2d.h
    GeomAPI_Tools.h
    GeomAPI_Shell.h
    GeomAPI_Solid.h
    GeomAPI_Sphere.h
    GeomAPI_Cylinder.h
    GeomAPI_Cone.h
    GeomAPI_Torus.h
    GeomAPI_Box.h
)

SET(PROJECT_SOURCES
    GeomAPI_Circ.cpp
    GeomAPI_Circ2d.cpp
    GeomAPI_Interface.cpp
    GeomAPI_XY.cpp
    GeomAPI_XYZ.cpp
    GeomAPI_Pnt.cpp
    GeomAPI_Pnt2d.cpp
    GeomAPI_Lin.cpp
    GeomAPI_Lin2d.cpp
    GeomAPI_Dir.cpp
    GeomAPI_Dir2d.cpp
    GeomAPI_Pln.cpp
    GeomAPI_Shape.cpp
    GeomAPI_ShapeExplorer.cpp
    GeomAPI_ShapeIterator.cpp
    GeomAPI_Edge.cpp
    GeomAPI_Face.cpp
    GeomAPI_PlanarEdges.cpp
    GeomAPI_AISObject.cpp
    GeomAPI_Curve.cpp
    GeomAPI_DataMapOfShapeMapOfShapes.cpp
    GeomAPI_DataMapOfShapeShape.cpp
    GeomAPI_Vertex.cpp
    GeomAPI_ICustomPrs.cpp
    GeomAPI_Ax1.cpp
    GeomAPI_Ax2.cpp
    GeomAPI_Ax3.cpp
    GeomAPI_IPresentable.cpp
    GeomAPI_Trsf.cpp
    GeomAPI_Angle2d.cpp
    GeomAPI_Angle.cpp
    GeomAPI_Wire.cpp
    GeomAPI_Ellipse.cpp
    GeomAPI_Ellipse2d.cpp
    GeomAPI_Tools.cpp
    GeomAPI_Shell.cpp
    GeomAPI_Solid.cpp
    GeomAPI_Sphere.cpp
    GeomAPI_Cylinder.cpp
    GeomAPI_Cone.cpp
    GeomAPI_Torus.cpp
    GeomAPI_Box.cpp
)

SET(PROJECT_LIBRARIES
    ${OpenCASCADE_FoundationClasses_LIBRARIES}
    ${OpenCASCADE_ModelingAlgorithms_LIBRARIES}
    ${OpenCASCADE_Visualization_LIBRARIES}
)

ADD_DEFINITIONS(-DGEOMAPI_EXPORTS ${OpenCASCADE_DEFINITIONS})
ADD_LIBRARY(GeomAPI SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})

SET(CMAKE_SWIG_FLAGS -threads -Wall)

SET_SOURCE_FILES_PROPERTIES(GeomAPI.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(GeomAPI.i PROPERTIES SWIG_DEFINITIONS "-shadow")

INCLUDE_DIRECTORIES(
  ${OpenCASCADE_INCLUDE_DIR}
)

TARGET_LINK_LIBRARIES(GeomAPI ${PROJECT_LIBRARIES})

SET(SWIG_SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/GeomAPI.py
)

SET(SWIG_LINK_LIBRARIES
  GeomAPI
  ${PYTHON_LIBRARIES}
)

IF(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
  SWIG_ADD_LIBRARY(GeomAPI LANGUAGE python SOURCES GeomAPI.i ${PROJECT_HEADERS})
ELSE()
  SWIG_ADD_MODULE(GeomAPI python GeomAPI.i ${PROJECT_HEADERS})
ENDIF()
SWIG_LINK_LIBRARIES(GeomAPI ${SWIG_LINK_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_GeomAPI PROPERTIES DEBUG_OUTPUT_NAME _GeomAPI_d)
ENDIF(WIN32)

INSTALL(TARGETS _GeomAPI DESTINATION ${SHAPER_INSTALL_SWIG})
INSTALL(TARGETS GeomAPI DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${SWIG_SCRIPTS} DESTINATION ${SHAPER_INSTALL_SWIG})

ADD_UNIT_TESTS(
  TestAx123.py
  TestBox.py
  TestCone.py
  TestCylinder.py
  TestEllipse2d.py
  TestLine.py
  TestPlanarEdge.py
  TestPolygon.py
  TestSphere.py
  TestTorus.py
  Test2675.py
)
