# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Point_2 = model.addPoint(Part_1_doc, model.selection("EDGE", "Box_1_1/Front&Box_1_1/Left"), 0.4, True, False)
model.do()
model.end()
