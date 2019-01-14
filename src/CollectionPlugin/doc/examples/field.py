# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Field_1_objects = [model.selection("FACE", "Box_1_1/Left"), model.selection("FACE", "Box_1_1/Top"), model.selection("FACE", "Box_1_1/Front")]
Field_1 = model.addField(Part_1_doc, 2, "DOUBLE", 2, ["Comp 1", "Comp 2"], Field_1_objects)
Field_1.addStep(0, 0, [[0, 0], [0.1, 0.2], [2.1, 1.7], [3.7, 1.95]])
Field_1.addStep(1, 0, [[0, 0], [0.1, 0.2], [1.1, 2.1], [0.75, 3.1]])
model.do()
model.end()
