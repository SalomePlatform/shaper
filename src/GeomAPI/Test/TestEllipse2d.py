# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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

import math

from GeomAPI import *

TOLERANCE = 1.e-7

center = GeomAPI_Pnt2d(10, 0)
xAxis = GeomAPI_Dir2d(1, 0)
majorRadius = 10
minorRadius = 5

# Test 1. Default ellipse
ellipse = GeomAPI_Ellipse2d(center, xAxis, majorRadius, minorRadius)
assert(ellipse.center().distance(center) < TOLERANCE)
assert(ellipse.firstFocus().distance(GeomAPI_Pnt2d(18.66025404, 0.)) < TOLERANCE)
assert(ellipse.secondFocus().distance(GeomAPI_Pnt2d(1.339745962, 0.)) < TOLERANCE)
assert(ellipse.majorRadius() == majorRadius)
assert(ellipse.minorRadius() == minorRadius)

# Test 2. Ellipse with swapped radii
ellipse = GeomAPI_Ellipse2d(center, xAxis, minorRadius, majorRadius)
assert(ellipse.center().distance(center) < TOLERANCE)
assert(ellipse.firstFocus().distance(GeomAPI_Pnt2d(10., 8.66025404)) < TOLERANCE)
assert(ellipse.secondFocus().distance(GeomAPI_Pnt2d(10., -8.66025404)) < TOLERANCE)
assert(ellipse.majorRadius() == majorRadius)
assert(ellipse.minorRadius() == minorRadius)

# Test 3. Ellipse by 3 points
axisPnt = GeomAPI_Pnt2d(center.x() + majorRadius, center.y())
passedPnt = GeomAPI_Pnt2d(center.x(), center.y() + minorRadius)
ellipse = GeomAPI_Ellipse2d(center, axisPnt, passedPnt)
assert(ellipse.center().distance(center) < TOLERANCE)
assert(ellipse.firstFocus().distance(GeomAPI_Pnt2d(18.66025404, 0.)) < TOLERANCE)
assert(ellipse.secondFocus().distance(GeomAPI_Pnt2d(1.339745962, 0.)) < TOLERANCE)
assert(ellipse.majorRadius() == majorRadius)
assert(ellipse.minorRadius() == minorRadius)
