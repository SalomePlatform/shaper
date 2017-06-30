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
edgx = edgeBuilder.line(1., 0., 0.)
edgy = edgeBuilder.line(0., 1., 0.)
edgz = edgeBuilder.line(0., 0., 1.)
edg1 = edgeBuilder.line(0., 10., 0.)
edgaxis = edgeBuilder.line(ax1.dir().x(), ax1.dir().y(), ax1.dir().z())

Cylinder_1 = shaperpy.makeCylinder(5., 10.)
Cylinder_2 = shaperpy.makeCylinder(pnt2, edgx, 5., 10.)
Cylinder_3 = shaperpy.makeCylinder(pnt2, edg1, 7., 12.)

try:
  Cylinder_4 = shaperpy.makeCylinder(0., 10.)
except myExcept,ec:
  assert(ec.what() == "Cylinder builder :: radius is negative or null.")

try:
  Cylinder_5 = shaperpy.makeCylinder(-5., 10.)
except myExcept,ec:
  assert(ec.what() == "Cylinder builder :: radius is negative or null.")

try:
  Cylinder_6 = shaperpy.makeCylinder(5., 0.)
except myExcept,ec:
  assert(ec.what() == "Cylinder builder :: height is negative or null.")

try:
  Cylinder_7 = shaperpy.makeCylinder(5., -10.)
except myExcept,ec:
  assert(ec.what() == "Cylinder builder :: height is negative or null.")

try:
  Cylinder_8 = shaperpy.makeCylinder(None, edgz, 5., 10.)
except myExcept,ec:
  assert(ec.what() == "Cylinder builder :: the base point is not valid.")

try:
  Cylinder_9 = shaperpy.makeCylinder(pnt1, None, 5., 10.)
except myExcept,ec:
  assert(ec.what() == "Cylinder builder :: the axis is not valid.")

Cylinder_14 = shaperpy.makeCylinder(5., 10., 45.)
Cylinder_15 = shaperpy.makeCylinder(pnt2, edgx, 5., 10., 90.)
Cylinder_16 = shaperpy.makeCylinder(pnt2, edgaxis, 7., 12., 120.)
Cylinder_17 = shaperpy.makeCylinder(pnt2, edgy, 5., 10., 360.)

try:
  Cylinder_18 = shaperpy.makeCylinder(0., 10., 45.)
except myExcept,ec:
  assert(ec.what() == "Cylinder builder :: radius is negative or null.")

try:
  Cylinder_19 = shaperpy.makeCylinder(-5., 10., 45.)
except myExcept,ec:
  assert(ec.what() == "Cylinder builder :: radius is negative or null.")

try:
  Cylinder_20 = shaperpy.makeCylinder(5., 0., 45.)
except myExcept,ec:
  assert(ec.what() == "Cylinder builder :: height is negative or null.")

try:
  Cylinder_21 = shaperpy.makeCylinder(5., -10., 45.)
except myExcept,ec:
  assert(ec.what() == "Cylinder builder :: height is negative or null.")

try:
  Cylinder_22 = shaperpy.makeCylinder(5., 10., 0.)
except myExcept,ec:
  assert(ec.what() == "Cylinder builder :: angle is negative or null.")

try:
  Cylinder_23 = shaperpy.makeCylinder(5., 10., -45.)
except myExcept,ec:
  assert(ec.what() == "Cylinder builder :: angle is negative or null.")

try:
  Cylinder_24 = shaperpy.makeCylinder(5., 10., 450.)
except myExcept,ec:
  assert(ec.what() == "Cylinder builder :: angle greater than 360 degrees.")

try:
  Cylinder_25 = shaperpy.makeCylinder(None, edgz, 5., 10., 90.)
except myExcept,ec:
  assert(ec.what() == "Cylinder builder :: the base point is not valid.")

try:
  Cylinder_26 = shaperpy.makeCylinder(pnt1, None, 5., 10., 90.)
except myExcept,ec:
  assert(ec.what() == "Cylinder builder :: the axis is not valid.")