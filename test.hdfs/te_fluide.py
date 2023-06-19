# Copyright (C) 2014-2023  CEA, EDF
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
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.SOLID, [30])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.FACE, [231])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.EDGE, [968])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.VERTEX, [1936])
  model.testResultsVolumes(aPartFeature, [1429610.103267467])
