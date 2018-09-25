# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(24.1, 24.5, 18.2)
SketchLine_1 = Sketch_1.addLine(62.3, 85.1, 91.2, 56.4)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchCircle_1.center(), SketchLine_1.startPoint(), 75.5)
model.do()
model.end()