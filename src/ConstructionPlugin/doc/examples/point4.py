# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Axis_4 = model.addAxis(Part_1_doc, model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Bottom]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Top]"))
Axis_5 = model.addAxis(Part_1_doc, model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Right][Box_1_1/Bottom]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Top]"))
Point_2 = model.addPoint(Part_1_doc, model.selection("EDGE", "Axis_1"), model.selection("EDGE", "Axis_2"))
model.do()
model.end()
