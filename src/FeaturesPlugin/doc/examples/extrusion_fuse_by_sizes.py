# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
ExtrusionFuse_1 = model.addExtrusionFuse(Part_1_doc,
                                         [],
                                         model.selection(),
                                         0,
                                         5,
                                         [model.selection("SOLID", "Box_1_1")])
Sketch_1 = model.addSketch(Part_1_doc,
                           model.selection("FACE", "Box_1_1/Top"))
SketchCircle_1 = Sketch_1.addCircle(5, 0, 2)
ExtrusionFuse_1.setNestedSketch(Sketch_1)
model.do()
model.end()
