# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchArc_1 = Sketch_1.addArc(6.0, 6.2, 5.2, 34.3, 34.0, 7.7, True)
SketchArc_2 = Sketch_1.addArc(8.1, 56.7, 58.2, 6.6, 44.8, 69.2)
SketchLine_3 = Sketch_1.addLine(25.0, 109.4, 68.1, 153.6)
SketchLine_3.setAuxiliary(True)
SketchArc_3 = Sketch_1.addArc(SketchLine_3.startPoint(), 92.1, 34.0, True)
model.do()
model.end()
