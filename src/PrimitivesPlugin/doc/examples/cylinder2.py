# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cylinder_2 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "Cylinder_1_1/Face_1&Cylinder_1_1/Face_2"), model.selection("EDGE", "PartSet/OX"), 5, 10, 45)
model.do()
model.end()
