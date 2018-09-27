# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Point_2 = model.addPoint(Part_1_doc, 10, 10, 10)
Point_3 = model.addPoint(Part_1_doc, model.selection("VERTEX", "Point_1"), model.selection("FACE", "PartSet/XOY"))
model.do()
model.end()
