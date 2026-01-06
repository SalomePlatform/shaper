# Copyright (C) 2025-2026  CEA, EDF
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

if __name__ == "__main__":
  aPartFeature = locals()["Part_1"]

  model.testNbResults(aPartFeature, 1)
  model.testNbSubResults(aPartFeature, [0])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.SOLID, [0])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.FACE, [1])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.EDGE, [2])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.VERTEX, [4])

  model.testResultsLengths(aPartFeature, [151.87135], 6)
  model.testResultsAreas(aPartFeature, [833.33333], 6)

  from ModelHighAPI import CHECK_NAMING

  # With the commit for [bos #44274], the plane size uses now correctly the preference value.
  # This old HDF stores old wrong plane size values
  # So this check prevents test to fail during check YOZ, XOZ, XOY plane's sizes, we simply ignore them
  checkPlanes = False
  assert(model.checkPythonDump(CHECK_NAMING, checkPlanes))
