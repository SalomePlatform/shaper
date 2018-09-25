# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Point_2 = model.addPoint(Part_1_doc, 50, 50, 50)
Point_3 = model.addPoint(Part_1_doc, model.selection("EDGE", "Box_1_1/Left&Box_1_1/Top"), 0.5, True, False)
Point_4 = model.addPoint(Part_1_doc, model.selection("VERTEX", "Box_1_1/Front&Box_1_1/Left&Box_1_1/Top"), model.selection("EDGE", "Box_1_1/Right&Box_1_1/Top"))
Point_5 = model.addPoint(Part_1_doc, model.selection("EDGE", "Box_1_1/Front&Box_1_1/Top"), model.selection("FACE", "Box_1_1/Left"), 10, False)
Point_6 = model.addPoint(Part_1_doc, model.selection("SOLID", "Box_1_1"))
model.do()
model.end()
