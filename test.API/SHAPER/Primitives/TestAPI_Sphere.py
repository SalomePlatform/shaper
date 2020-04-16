# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

# Points
pnt1 = pnt(10., 10., 10.)

Sphere_1 = shaperpy.makeSphere(pnt1, 10.)
Sphere_2 = shaperpy.makeSphere(10.)

try:
  Sphere_3 = shaperpy.makeSphere(pnt1, 0.)
except myExcept as ec:
  assert(ec.what() == "Sphere builder :: radius is negative or null.")

try:
  Sphere_4 = shaperpy.makeSphere(0.)
except myExcept as ec:
  assert(ec.what() == "Sphere builder :: radius is negative or null.")

try:
  Sphere_5 = shaperpy.makeSphere(None, 10.)
except myExcept as ec:
  assert(ec.what() == "Sphere builder :: center is not valid.")
