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
from GeomAlgoAPI import GeomAlgoAPI_EdgeBuilder as edgeBuilder
from GeomAPI import GeomAPI_Pnt as pnt
from GeomAPI import GeomAPI_Ax1 as axis
from GeomAPI import GeomAPI_Dir as direction

# Points
pnt1 = pnt(0., 0., 0.)
pnt2 = pnt(10., 10., 10.)

# Axis
yDir = direction(0.,10.,0.)
ax1 = axis(pnt1, yDir)

# Edges
edgaxis = edgeBuilder.line(ax1.dir().x(), ax1.dir().y(), ax1.dir().z())

Cone_1 = shaperpy.makeCone(5., 10., 10.)
Cone_2 = shaperpy.makeCone(0., 10., 10.)
Cone_3 = shaperpy.makeCone(5., 0., 10.)

try:
  Cone_4 = shaperpy.makeCone(5., 10., 0.)
except myExcept as ec:
  assert(ec.what() == "Cone builder :: height is negative or null.")

Cone_5 = shaperpy.makeCone(pnt2, edgaxis, 5., 10., 10.)
Cone_6 = shaperpy.makeCone(pnt2, edgaxis, 0., 10., 10.)
Cone_7 = shaperpy.makeCone(pnt2, edgaxis, 5., 0., 10.)

try:
  Cone_8 = shaperpy.makeCone(None, edgaxis, 5., 10., 10.)
except myExcept as ec:
  assert(ec.what() == "Cone builder :: the base point is not valid.")

try:
  Cone_9 = shaperpy.makeCone(pnt2, None, 5., 10., 10.)
except myExcept as ec:
  assert(ec.what() == "Cone builder :: the axis is not valid.")

try:
  Cone_10 = shaperpy.makeCone(pnt2, edgaxis, 5., 10., 0.)
except myExcept as ec:
  assert(ec.what() == "Cone builder :: height is negative or null.")
