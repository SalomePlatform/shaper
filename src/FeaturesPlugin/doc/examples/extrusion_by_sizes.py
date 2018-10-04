# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(0, 0, 25)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc,
                                 [model.selection("COMPOUND", "Sketch_1")],
                                 model.selection(),
                                 50,
                                 0)
model.do()
model.end()
