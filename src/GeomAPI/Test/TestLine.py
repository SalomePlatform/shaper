# Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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

loc = GeomAPI_Pnt2d(10, 0)
axis = GeomAPI_Dir2d(1, 0)

# Test 1. GeomAPI_Lin2d
line2d = GeomAPI_Lin2d(loc, axis)
assert(line2d.location().distance(loc) < TOLERANCE)
assert(line2d.isRight(GeomAPI_Pnt2d(15, 5)))

# Test 2. GeomAPI_Lin
ln1 = GeomAPI_Lin(10, 0, 0, 20, 0, 0)
ln2 = GeomAPI_Lin(15, -1, 1, 15, 1, 1)
assert(ln1.isCoplanar(ln2) == False)
assert(ln1.contains(None, TOLERANCE) == False)
