# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), 10)
Scale_1 = model.addScale(Part_1_doc, [model.selection("SOLID", "Sphere_1_1")] , model.selection("VERTEX", "PartSet/Origin"), 2 , 3, 4)
model.do()
model.end()
