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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-53.81455412066646, -50.08190818345371, 33.23044991620489, -41.72362634869452)
SketchArc_1 = Sketch_1.addArc(-10.71003639844217, -41.54978593924657, 33.23044991620489, -41.72362634869452, -53.81455412066646, -50.08190818345371, False)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.endPoint())
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1")], False)
Wire_2 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchArc_1_2")], False)
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Wire_1_1"), model.selection("WIRE", "Wire_2_1")])
Wire_3 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1")], False)

model.end()

from ModelAPI import *
factory = ModelAPI_Session.get().validators()
assert(factory.validate(Wire_3.feature()))

from GeomAPI import *

model.testNbResults(Part_1, 1)
model.testNbSubResults(Part_1, [0])
model.testNbSubShapes(Part_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Part_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Part_1, GeomAPI_Shape.EDGE, [3])
model.testNbSubShapes(Part_1, GeomAPI_Shape.VERTEX, [6])
model.testResultsAreas(Part_1, [3416.5596])

assert(model.checkPythonDump())
