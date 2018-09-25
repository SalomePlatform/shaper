# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Solid_1_objects = [model.selection("FACE", "Box_1_1/Top"),
                   model.selection("FACE", "Box_1_1/Front"),
                   model.selection("FACE", "Box_1_1/Left"),
                   model.selection("FACE", "Box_1_1/Back"),
                   model.selection("FACE", "Box_1_1/Right"),
                   model.selection("FACE", "Box_1_1/Bottom")]
Solid_1 = model.addSolid(Part_1_doc, Solid_1_objects)
model.do()
model.end()
