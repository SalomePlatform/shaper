# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(13, 58, 69, 75)
SketchLine_2 = Sketch_1.addLine(12, 38, 73, 23)
SketchConstraintParallel_1 = Sketch_1.setPerpendicular(SketchLine_1.result(), SketchLine_2.result())
model.do()
model.end()