# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Fillet_1 = model.addFillet(Part_1_doc, [model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]")], 1, 3)
model.do()
model.end()
