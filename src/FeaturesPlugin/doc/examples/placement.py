# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Front"))
SketchCircle_1 = Sketch_1.addCircle(20, 6, 3)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc,
                                 [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f")],
                                 model.selection(), -10, 0)
Placement_1 = model.addPlacement(Part_1_doc,
                                 [model.selection("SOLID", "Extrusion_1_1")],
                                 model.selection("FACE", "Extrusion_1_1/From_Face_1"),
                                 model.selection("FACE", "Box_1_1/Front"), False, True)
model.do()
model.end()
