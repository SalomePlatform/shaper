## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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

import math

from GeomAlgoAPI import GeomAlgoAPI_Circ2dBuilder as circBuilder
from GeomAlgoAPI import GeomAlgoAPI_CurveBuilder as curveBuilder
from GeomAlgoAPI import GeomAlgoAPI_EdgeBuilder as edgeBuilder
from GeomAlgoAPI import GeomAlgoAPI_FaceBuilder as faceBuilder
from GeomAlgoAPI import GeomAlgoAPI_ShapeTools as tools
from GeomAlgoAPI import GeomAlgoAPI_MakeVolume as makeVolume
from GeomAPI import GeomAPI_Ax3 as ax3
from GeomAPI import GeomAPI_Dir as dir
from GeomAPI import GeomAPI_Pnt as pnt
from GeomAPI import GeomAPI_Pnt2d as pnt2d
from GeomAPI import GeomAPI_Circ2d
from GeomAPI import GeomAPI_Shape

TOLERANCE = 1.e-7

center = pnt(10, 0, 0)
normal = dir(0, 0, 1)
xAxis = dir(1, 0, 0)
majorRadius = 10
minorRadius = 5

# Test 1. Elliptical edge
ellipseEdge = edgeBuilder.ellipse(center, normal, xAxis, majorRadius, minorRadius)
ellipse = ellipseEdge.ellipse()
assert(ellipse is not None)
assert(ellipse.center().distance(center) < TOLERANCE)
assert(ellipse.firstFocus().distance(pnt(18.66025404, 0., 0.)) < TOLERANCE)
assert(ellipse.secondFocus().distance(pnt(1.339745962, 0., 0.)) < TOLERANCE)
assert(ellipse.majorRadius() == majorRadius)
assert(ellipse.minorRadius() == minorRadius)

# Test 2. Square face
size = 100.
squareFace = faceBuilder.squareFace(center, normal, size)
assert(squareFace.middlePoint().distance(center) < TOLERANCE)

# Test 3. Check are of the face
assert(tools.area(squareFace) == size**2)

# Test 4. Check properties of wrong shapes
assert(tools.volume(None) == 0)
assert(tools.volume(GeomAPI_Shape()) == 0)
assert(tools.area(None) == 0)
assert(tools.area(GeomAPI_Shape()) == 0)
assert(tools.centreOfMass(None) is None)
assert(tools.centreOfMass(GeomAPI_Shape()) is None)

# Test 5. Curve with duplicated points
points = [pnt(10, 0, 0),
          pnt(20, 10, 0),
          pnt(20, 10, 0),
          pnt(10, 10, 0),
          pnt(10, 0, 0)]
curve = curveBuilder.edge(points, True, True, None, None)
assert(curve is not None)

# Test 6. Circle by center and radius
center = pnt2d(5, 0)
radius = 5
plane = ax3(pnt(0, 0, 0), dir(0, 0, 1), dir(1, 0, 0))
circleBuilder = circBuilder(plane)
circleBuilder.setCenter(center)
circleBuilder.setRadius(radius)
circle = circBuilder.circle(circleBuilder)
assert(circle is not None)
assert(circle.center().distance(center) < TOLERANCE)
assert(circle.radius() == radius)

# Test 7. Circle by 3 points
p1 = pnt2d(center.x() + radius, center.y())
p2 = pnt2d(center.x() - radius, center.y())
p3 = pnt2d(center.x(), center.y() + radius)
circle = circBuilder.circle(p1, p2, p3)
assert(circle is not None)
assert(circle.center().distance(center) < TOLERANCE)
assert(circle.radius() == radius)
