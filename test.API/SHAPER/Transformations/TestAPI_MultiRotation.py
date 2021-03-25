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

from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Ax1 as axis, GeomAPI_Pnt as pnt, GeomAPI_Dir as direction

# Create Boxes
Box_1 = shaperpy.makeBox(10.,10.,10.)
Box_2 = shaperpy.makeBox(10.,10.,10.)
Box_3 = shaperpy.makeBox(10.,10.,10.)
Box_4 = shaperpy.makeBox(10.,10.,10.)
Box_5 = shaperpy.makeBox(10.,10.,10.)
Box_6 = shaperpy.makeBox(10.,10.,10.)
Box_7 = shaperpy.makeBox(10.,10.,10.)

# Points
pntOrigin = pnt(0.,0.,0.)
pnt1 = pnt(10.,0.,0.)

# Axis
xDir = direction(10., 0., 0.)
ax1 = axis(pntOrigin, xDir)

# Create MultiRotations
MultiRotation_1 = shaperpy.makeMultiRotation(Box_1, ax1, 45., 5)
MultiRotation_2 = shaperpy.makeMultiRotation(Box_2, ax1, 0., 5)

try:
  MultiRotation_3 = shaperpy.makeMultiRotation(Box_3, ax1, 45., -5)
except myExcept as ec:
  assert(ec.what() == "Multirotation builder :: the number of copies is null or negative.")

try:
  MultiRotation_4 = shaperpy.makeMultiRotation(None, ax1, 45., 5)
except myExcept as ec:
  assert(ec.what() == "Transformation :: incorrect input data.")

try:
  MultiRotation_5 = shaperpy.makeMultiRotation(Box_4, None, 45., 5)
except myExcept as ec:
  assert(ec.what() == "Multirotation builder :: the axis is not valid")

MultiRotation_6 = shaperpy.makeMultiRotation(Box_5, ax1, 5)

try:
  MultiRotation_7 = shaperpy.makeMultiRotation(Box_6, ax1, -5)
except myExcept as ec:
  assert(ec.what() == "Multirotation builder :: the number of copies is null or negative.")

try:
  MultiRotation_8 = shaperpy.makeMultiRotation(None, ax1, 5)
except myExcept as ec:
  assert(ec.what() == "Transformation :: incorrect input data.")

try:
  MultiRotation_9 = shaperpy.makeMultiRotation(Box_7, None, 5)
except myExcept as ec:
  assert(ec.what() == "Multirotation builder :: the axis is not valid")
