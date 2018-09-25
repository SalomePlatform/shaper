# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Symmetry_1 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")], model.selection("FACE", "Cylinder_1_1/Face_2"), True)
model.do()
model.end()
