# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Plane_4 = model.addPlane(Part_1_doc, model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"), model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Right][Box_1_1/Bottom]"), False)
model.do()
model.end()
