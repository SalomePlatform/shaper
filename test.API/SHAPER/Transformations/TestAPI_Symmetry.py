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

from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Pnt as pnt
from GeomAPI import GeomAPI_Dir as direction
from GeomAPI import GeomAPI_Ax1 as axis
from GeomAPI import GeomAPI_Ax2 as plane
from GeomAPI import GeomAPI_Shape as shape

# Create boxes
Box_1 = shaperpy.makeBox(10., 10., 10.)
Box_2 = shaperpy.makeBox(10., 10., 10.)
Box_3 = shaperpy.makeBox(10., 10., 10.)
Box_4 = shaperpy.makeBox(10., 10., 10.)
Box_5 = shaperpy.makeBox(10., 10., 10.)
Box_6 = shaperpy.makeBox(10., 10., 10.)
Box_7 = shaperpy.makeBox(10., 10., 10.)
Box_8 = shaperpy.makeBox(10., 10., 10.)
Box_9 = shaperpy.makeBox(10., 10., 10.)
Box_10 = shaperpy.makeBox(10., 10., 10.)
Box_11 = shaperpy.makeBox(10., 10., 10.)
Box_12 = shaperpy.makeBox(10., 10., 10.)
Box_13 = shaperpy.makeBox(10., 10., 10.)
Box_14 = shaperpy.makeBox(10., 10., 10.)
Box_15 = shaperpy.makeBox(10., 10., 10.)
Box_16 = shaperpy.makeBox(10., 10., 10.)
Box_17 = shaperpy.makeBox(10., 10., 10.)
Box_18 = shaperpy.makeBox(10., 10., 10.)

# Create points
origin = pnt(0., 0., 0.)
pnt1 = pnt(-10., -10., -10)

# Create Axis
dirZ = direction(0., 0., 10.)
axZ = axis(origin, dirZ)

dir1 = direction(10., 10., 10.)
ax1 = axis(origin, dir1)

# Create Planes
planeXOY = plane(origin, dirZ)

Symmetry_1 = shaperpy.makeSymmetry(Box_1, origin)
Symmetry_2 = shaperpy.makeSymmetry(Box_2, pnt1)

try :
    Symmetry_3 = shaperpy.makeSymmetry(Box_3, None)
except myExcept as ec :
    assert(ec.what() == "Symmetry builder :: point is not valid.")

Symmetry_4 = shaperpy.makeSymmetry(Box_4, axZ)
Symmetry_5 = shaperpy.makeSymmetry(Box_5, ax1)
Symmetry_6 = shaperpy.makeSymmetry(Box_6, planeXOY)

try :
    Symmetry_7 = shaperpy.makeSymmetry(None, pnt1)
except myExcept as ec :
    assert(ec.what() == "Transformation :: incorrect input data.")

try :
    Symmetry_8 = shaperpy.makeSymmetry(None, axZ)
except myExcept as ec :
    assert(ec.what() == "Transformation :: incorrect input data.")

try :
    Symmetry_9 = shaperpy.makeSymmetry(None, planeXOY)
except myExcept as ec :
    assert(ec.what() == "Transformation :: incorrect input data.")

try :
    Symmetry_10 = shaperpy.makeSymmetry(shape(), pnt1)
except myExcept as ec :
    assert(ec.what() == "Transformation :: source shape does not contain any actual shape.")
