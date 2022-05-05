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
from GeomAPI import GeomAPI_Ax1 as axis, GeomAPI_Pnt as pnt, GeomAPI_Dir as direction
from GeomAPI import GeomAPI_Shape as shape

# Create Boxes
Box_1 = shaperpy.makeBox(10.,10.,10.)
Box_2 = shaperpy.makeBox(10.,10.,10.)
Box_3 = shaperpy.makeBox(10.,10.,10.)
Box_4 = shaperpy.makeBox(10.,10.,10.)
Box_5 = shaperpy.makeBox(10.,10.,10.)
Box_6 = shaperpy.makeBox(10.,10.,10.)
Box_7 = shaperpy.makeBox(10.,10.,10.)
Box_8 = shaperpy.makeBox(10.,10.,10.)
Box_9 = shaperpy.makeBox(10.,10.,10.)
Box_10 = shaperpy.makeBox(10.,10.,10.)
Box_11 = shaperpy.makeBox(10.,10.,10.)
Box_12 = shaperpy.makeBox(10.,10.,10.)
Box_13 = shaperpy.makeBox(10.,10.,10.)
Box_14 = shaperpy.makeBox(10.,10.,10.)
Box_15 = shaperpy.makeBox(10.,10.,10.)

# Points
pntOrigin = pnt(0.,0.,0.)
pnt1 = pnt(20.,0.,0.)
pnt2 = pnt(20.,20.,0.)
pnt3 = pnt(-10.,0.,0.)

# Axis
zDir = direction(0., 0., 10.)
ax1 = axis(pntOrigin, zDir)


# Create Rotations
Rotation_1 = shaperpy.makeRotation(Box_1, ax1, 45)
Rotation_2 = shaperpy.makeRotation(Box_2, ax1, 0)
Rotation_3 = shaperpy.makeRotation(Box_3, ax1, 360)
Rotation_4 = shaperpy.makeRotation(Box_4, ax1, -360)

try:
    Rotation_5 = shaperpy.makeRotation(Box_5, ax1, 450)
except myExcept as ec:
    assert(ec.what() == "Rotation builder :: angle greater than 360 degrees.")

try:
    Rotation_6 = shaperpy.makeRotation(Box_6, ax1, -450)
except myExcept as ec:
    assert(ec.what() == "Rotation builder :: angle smaller than -360 degrees.")

try:
    Rotation_7 = shaperpy.makeRotation(Box_7, None, 180)
except myExcept as ec:
    assert(ec.what() == "Rotation builder :: axis is not valid.")

try:
    Rotation_8 = shaperpy.makeRotation(None, ax1, 180)
except myExcept as ec:
    assert(ec.what() == "Transformation :: incorrect input data.")

Rotation_9 = shaperpy.makeRotation(Box_8, pntOrigin, pnt1, pnt2)
Rotation_10 = shaperpy.makeRotation(Box_9, pnt3, pnt1, pnt2)

try:
    Rotation_11 = shaperpy.makeRotation(Box_10, pnt3, pnt1, pnt1)
except myExcept as ec:
    assert(ec.what() == "Rotation builder :: start point and end point coincide.")

try:
    Rotation_12 = shaperpy.makeRotation(Box_11, pnt3, pnt3, pnt1)
except myExcept as ec:
    assert(ec.what() == "Rotation builder :: center point and start point coincide.")

try:
    Rotation_13 = shaperpy.makeRotation(Box_12, pnt3, pnt1, pnt3)
except myExcept as ec:
    assert(ec.what() == "Rotation builder :: center point and end point coincide.")

try:
    Rotation_14 = shaperpy.makeRotation(Box_13, pntOrigin, pnt1, None)
except myExcept as ec:
    assert(ec.what() == "Rotation builder :: end point is not valid.")

try:
    Rotation_15 = shaperpy.makeRotation(Box_14, None, pnt3, pnt2)
except myExcept as ec:
    assert(ec.what() == "Rotation builder :: center point is not valid.")

try:
    Rotation_16 = shaperpy.makeRotation(Box_15, pntOrigin, None, pnt1)
except myExcept as ec:
    assert(ec.what() == "Rotation builder :: start point is not valid.")

try:
    Rotation_17 = shaperpy.makeRotation(None, pntOrigin, pnt3, pnt2)
except myExcept as ec:
    assert(ec.what() == "Transformation :: incorrect input data.")

try:
    Rotation_18 = shaperpy.makeRotation(shape(), ax1, 450)
except myExcept as ec:
    assert(ec.what() == "Transformation :: source shape does not contain any actual shape.")
