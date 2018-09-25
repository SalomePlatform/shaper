# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(7, 46, 63, 20)
SketchLine_2 = Sketch_1.addLine(63, 20, 55, 85)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
Fillet_1 = Sketch_1.setFillet(SketchLine_1.endPoint())
model.do()
model.end()
