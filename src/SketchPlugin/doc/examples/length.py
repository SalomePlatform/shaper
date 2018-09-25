# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(14.6, 14.2, 59.4, 61.9)
SketchLine_1.result().setColor(225, 0, 0)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 65.5)
model.do()
model.end()