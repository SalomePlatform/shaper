# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cone_1 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10, 5, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Cone_1_1")], model.selection("EDGE", "([Cone_1_1/Face_1][Cone_1_1/Face_2])([Cone_1_1/Face_1][Cone_1_1/Face_3])"), 40)
model.do()
model.end()
