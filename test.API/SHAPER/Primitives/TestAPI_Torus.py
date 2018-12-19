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

Torus_1 = shaperpy.makeTorus(10., 5.)

try:
  Torus_2 = shaperpy.makeTorus(0., 5.)
except myExcept as ec:
  assert(ec.what() == "Torus builder :: radius is negative or null.")

try:
  Torus_3 = shaperpy.makeTorus(10., 0.)
except myExcept as ec:
  assert(ec.what() == "Torus builder :: ring radius is negative or null.")

try:
  Torus_4 = shaperpy.makeTorus(5., 10.)
except myExcept as ec:
  assert(ec.what() == "Torus builder :: ring radius is greater than the radius.")

Torus_5 = shaperpy.makeTorus(pnt2, edgaxis, 10., 5.)

try:
  Torus_6 = shaperpy.makeTorus(None, edgaxis, 10., 5.)
except myExcept as ec:
  assert(ec.what() == "Torus builder :: the base point is not valid.")

try:
  Torus_7 = shaperpy.makeTorus(pnt2, None, 10., 5.)
except myExcept as ec:
  assert(ec.what() == "Torus builder :: the axis is not valid.")

try:
  Torus_8 = shaperpy.makeTorus(pnt2, edgaxis, 0., 5.)
except myExcept as ec:
  assert(ec.what() == "Torus builder :: radius is negative or null.")

try:
  Torus_8 = shaperpy.makeTorus(pnt2, edgaxis, 10., 0.)
except myExcept as ec:
  assert(ec.what() == "Torus builder :: ring radius is negative or null.")
