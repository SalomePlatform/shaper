# Copyright (C) 2018-2019  CEA/DEN, EDF R&D
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

from salome.shaper import model
from GeomAPI import GeomAPI_Shape

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Point_2 = model.addPoint(Part_1_doc, 10, 0, 0)
Point_3 = model.addPoint(Part_1_doc, 20, 0, 10)
Polyline_1 = model.addPolyline3D(Part_1_doc, [model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2")], False)

Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("EDGE", "Polyline_1_1/Generated_Edge&Point_1/Point_1")], model.selection("EDGE", "PartSet/OZ"), model.selection(), 0, model.selection("FACE", "PartSet/XOZ"), 0)

model.testNbResults(Revolution_1, 1)
model.testNbSubResults(Revolution_1, [0])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.VERTEX, [8])
model.testResultsVolumes(Revolution_1, [666.43244])

Revolution_1.setPlanesAndOffsets(model.selection("FACE", "PartSet/YOZ"), 0, model.selection("FACE", "PartSet/XOZ"), 0)

model.testNbResults(Revolution_1, 1)
model.testNbSubResults(Revolution_1, [0])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.VERTEX, [8])
model.testResultsVolumes(Revolution_1, [333.21622])

model.end()

assert(model.checkPythonDump())
