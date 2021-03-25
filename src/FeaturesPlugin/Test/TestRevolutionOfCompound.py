# Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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
SketchCircle_1 = Sketch_1.addCircle(15, -20, 8)
SketchCircle_2 = Sketch_1.addCircle(35, -35, 15)
model.do()
Compound_1 = model.addCompound(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r"), model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f")])
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1")], model.selection("EDGE", "PartSet/OX"), model.selection(), 0, model.selection("FACE", "PartSet/XOZ"), 0)
model.do()

Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Sketch_2 = model.addSketch(Part_2_doc, model.defaultPlane("XOY"))
SketchCircle_3 = Sketch_2.addCircle(15, -20, 8)
SketchCircle_4 = Sketch_2.addCircle(35, -35, 15)
model.do()
Compound_2 = model.addCompound(Part_2_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r"), model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f")])
Revolution_2 = model.addRevolution(Part_2_doc, [model.selection("COMPOUND", "Compound_1_1")], model.selection("EDGE", "PartSet/OX"), 90, 0)
model.do()

Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()
Sketch_3 = model.addSketch(Part_3_doc, model.defaultPlane("XOY"))
SketchCircle_5 = Sketch_3.addCircle(15, -20, 8)
SketchCircle_6 = Sketch_3.addCircle(35, -35, 15)
model.do()
Compound_3 = model.addCompound(Part_3_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r"), model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f")])
Revolution_3 = model.addRevolution(Part_3_doc, [model.selection("COMPOUND", "Compound_1_1")], model.selection("EDGE", "PartSet/OX"), model.selection("FACE", "PartSet/XOY"), 0, model.selection("FACE", "PartSet/XOZ"), 0)
model.do()

model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Revolution_1, 1)
model.testNbSubResults(Revolution_1, [0])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.VERTEX, [16])
model.testResultsVolumes(Revolution_1, [38861.56734363])

model.testNbResults(Revolution_2, 1)
model.testNbSubResults(Revolution_2, [2])
model.testNbSubShapes(Revolution_2, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Revolution_2, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Revolution_2, GeomAPI_Shape.EDGE, [12])
model.testNbSubShapes(Revolution_2, GeomAPI_Shape.VERTEX, [24])
model.testResultsVolumes(Revolution_2, [45178.11414513])

model.testNbResults(Revolution_3, 1)
model.testNbSubResults(Revolution_3, [0])
model.testNbSubShapes(Revolution_3, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Revolution_3, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(Revolution_3, GeomAPI_Shape.EDGE, [6])
model.testNbSubShapes(Revolution_3, GeomAPI_Shape.VERTEX, [12])
model.testResultsVolumes(Revolution_3, [38861.56733864])

assert(model.checkPythonDump())