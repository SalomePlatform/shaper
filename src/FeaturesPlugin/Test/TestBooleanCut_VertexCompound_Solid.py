# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchPoint_1 = Sketch_1.addPoint(-61.63122212692967, 34.13774013722127)
SketchPoint_2 = Sketch_1.addPoint(-27.26436963979418, 34.59596483704974)
SketchPoint_3 = Sketch_1.addPoint(25.43147084048027, 34.59596483704974)
SketchPoint_4 = Sketch_1.addPoint(54.29962692967411, 33.67951543739281)
model.do()
Compound_1_objects = [model.selection("VERTEX", "Sketch_1/SketchPoint_1"), model.selection("VERTEX", "Sketch_1/SketchPoint_2"), model.selection("VERTEX", "Sketch_1/SketchPoint_3"), model.selection("VERTEX", "Sketch_1/SketchPoint_4")]
Compound_1 = model.addCompound(Part_1_doc, Compound_1_objects)
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_2.addLine(42.11715050385933, 45.75316349699826, -43.32915483490565, 45.75316349699826)
SketchLine_2 = Sketch_2.addLine(-43.32915483490565, 45.75316349699826, -43.32915483490565, 20.90707471054887)
SketchLine_3 = Sketch_2.addLine(-43.32915483490565, 20.90707471054887, 42.11715050385933, 20.90707471054887)
SketchLine_4 = Sketch_2.addLine(42.11715050385933, 20.90707471054887, 42.11715050385933, 45.75316349699826)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 10, 10)
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1")], [model.selection("SOLID", "Extrusion_1_1")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [2])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [2])

assert(model.checkPythonDump())
