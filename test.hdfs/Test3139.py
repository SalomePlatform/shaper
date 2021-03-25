# Copyright (C) 2020-2021  CEA/DEN, EDF R&D
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
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.SOLID, [2])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.FACE, [16])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.EDGE, [74])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.VERTEX, [148])
  model.testResultsVolumes(aPartFeature, [23407.60956])

  aPartFeature = locals()["Part_2"]
  model.testNbResults(aPartFeature, 1)
  model.testNbSubResults(aPartFeature, [0])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.SOLID, [3])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.FACE, [19])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.EDGE, [80])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.VERTEX, [160])
  model.testResultsVolumes(aPartFeature, [39403.873983562])
