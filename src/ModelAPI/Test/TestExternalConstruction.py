# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

# The test for correct referencing for Part feature of whole construction result created in PartSet

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(-104, 73, 102, 265)
SketchLine_2 = Sketch_1.addLine(102, 265, 192, -1)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(192, -1, -104, 73)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
model.do()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", "PartSet/Sketch_1")], model.selection("EDGE", "PartSet/Sketch_1/SketchLine_3"), 360, 0)
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Revolution_1, 1)
model.testNbSubResults(Revolution_1, [0])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.VERTEX, [16])
model.testResultsVolumes(Revolution_1, [17830100])

assert(model.checkPythonDump())
