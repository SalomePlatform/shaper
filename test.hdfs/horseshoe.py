# Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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
  Part_1 = locals()["Part_1"]
  model.testNbResults(Part_1, 1)
  model.testNbSubResults(Part_1, [0])
  model.testNbSubShapes(Part_1, GeomAPI_Shape.SOLID, [1])
  model.testNbSubShapes(Part_1, GeomAPI_Shape.FACE, [20])
  model.testNbSubShapes(Part_1, GeomAPI_Shape.EDGE, [104])
  model.testNbSubShapes(Part_1, GeomAPI_Shape.VERTEX, [208])
  model.testResultsVolumes(Part_1, [937708.6935593])

  Part_2 = locals()["Part_2"]
  model.testNbResults(Part_2, 1)
  model.testNbSubResults(Part_2, [0])
  model.testNbSubShapes(Part_2, GeomAPI_Shape.SOLID, [1])
  model.testNbSubShapes(Part_2, GeomAPI_Shape.FACE, [12])
  model.testNbSubShapes(Part_2, GeomAPI_Shape.EDGE, [60])
  model.testNbSubShapes(Part_2, GeomAPI_Shape.VERTEX, [120])
  model.testResultsVolumes(Part_2, [256665.8235477])

  Part_3 = locals()["Part_3"]
  model.testNbResults(Part_3, 1)
  model.testNbSubResults(Part_3, [0])
  model.testNbSubShapes(Part_3, GeomAPI_Shape.SOLID, [1])
  model.testNbSubShapes(Part_3, GeomAPI_Shape.FACE, [12])
  model.testNbSubShapes(Part_3, GeomAPI_Shape.EDGE, [60])
  model.testNbSubShapes(Part_3, GeomAPI_Shape.VERTEX, [120])
  model.testResultsVolumes(Part_3, [2719045.0529109])

  Part_4 = locals()["Part_4"]
  model.testNbResults(Part_4, 1)
  model.testNbSubResults(Part_4, [0])
  model.testNbSubShapes(Part_4, GeomAPI_Shape.SOLID, [1])
  model.testNbSubShapes(Part_4, GeomAPI_Shape.FACE, [6])
  model.testNbSubShapes(Part_4, GeomAPI_Shape.EDGE, [24])
  model.testNbSubShapes(Part_4, GeomAPI_Shape.VERTEX, [48])
  model.testResultsVolumes(Part_4, [534313.7153742])

  Part_5 = locals()["Part_5"]
  model.testNbResults(Part_5, 1)
  model.testNbSubResults(Part_5, [0])
  model.testNbSubShapes(Part_5, GeomAPI_Shape.SOLID, [1])
  model.testNbSubShapes(Part_5, GeomAPI_Shape.FACE, [19])
  model.testNbSubShapes(Part_5, GeomAPI_Shape.EDGE, [96])
  model.testNbSubShapes(Part_5, GeomAPI_Shape.VERTEX, [192])
  model.testResultsVolumes(Part_5, [1111187.2858512])

  Part_6 = locals()["Part_6"]
  model.testNbResults(Part_6, 1)
  model.testNbSubResults(Part_6, [0])
  model.testNbSubShapes(Part_6, GeomAPI_Shape.SOLID, [1])
  model.testNbSubShapes(Part_6, GeomAPI_Shape.FACE, [12])
  model.testNbSubShapes(Part_6, GeomAPI_Shape.EDGE, [60])
  model.testNbSubShapes(Part_6, GeomAPI_Shape.VERTEX, [120])
  model.testResultsVolumes(Part_6, [124567.8141964])

  Part_7 = locals()["Part_7"]
  model.testNbResults(Part_7, 1)
  model.testNbSubResults(Part_7, [0])
  model.testNbSubShapes(Part_7, GeomAPI_Shape.SOLID, [1])
  model.testNbSubShapes(Part_7, GeomAPI_Shape.FACE, [12])
  model.testNbSubShapes(Part_7, GeomAPI_Shape.EDGE, [60])
  model.testNbSubShapes(Part_7, GeomAPI_Shape.VERTEX, [120])
  model.testResultsVolumes(Part_7, [3106355.9518294])

  Part_8 = locals()["Part_8"]
  model.testNbResults(Part_8, 1)
  model.testNbSubResults(Part_8, [0])
  model.testNbSubShapes(Part_8, GeomAPI_Shape.SOLID, [1])
  model.testNbSubShapes(Part_8, GeomAPI_Shape.FACE, [6])
  model.testNbSubShapes(Part_8, GeomAPI_Shape.EDGE, [24])
  model.testNbSubShapes(Part_8, GeomAPI_Shape.VERTEX, [48])
  model.testResultsVolumes(Part_8, [570471.5090269])

  Part_9 = locals()["Part_9"]
  model.testNbResults(Part_9, 1)
  model.testNbSubResults(Part_9, [0])
  model.testNbSubShapes(Part_9, GeomAPI_Shape.SOLID, [1])
  model.testNbSubShapes(Part_9, GeomAPI_Shape.FACE, [7])
  model.testNbSubShapes(Part_9, GeomAPI_Shape.EDGE, [30])
  model.testNbSubShapes(Part_9, GeomAPI_Shape.VERTEX, [60])
  model.testResultsVolumes(Part_9, [1688917.3777022])

  Part_10 = locals()["Part_10"]
  model.testNbResults(Part_10, 1)
  model.testNbSubResults(Part_10, [0])
  model.testNbSubShapes(Part_10, GeomAPI_Shape.SOLID, [1])
  model.testNbSubShapes(Part_10, GeomAPI_Shape.FACE, [10])
  model.testNbSubShapes(Part_10, GeomAPI_Shape.EDGE, [48])
  model.testNbSubShapes(Part_10, GeomAPI_Shape.VERTEX, [96])
  model.testResultsVolumes(Part_10, [6604147.6151849])
