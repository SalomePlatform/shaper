# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(16, 54, 10)
SketchLine_1 = Sketch_1.addLine(10, 10, 80, 80)
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchCircle_1.center(), SketchLine_1.result())
model.do()
model.end()
