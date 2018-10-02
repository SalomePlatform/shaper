# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(71, -15, -32, -15)
SketchLine_2 = Sketch_1.addLine(-32, -15, -32, -56)
SketchLine_3 = Sketch_1.addLine(-32, -56, 71, -56)
SketchLine_4 = Sketch_1.addLine(71, -56, 71, -15)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(3, -15, 36, -56)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_1.result())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_3.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc,
                                 [model.selection("FACE", "Sketch_1/Face-SketchLine_1f-SketchLine_2f-SketchLine_3f-SketchLine_5r"),
                                  model.selection("FACE", "Sketch_1/Face-SketchLine_1f-SketchLine_3f-SketchLine_4f-SketchLine_5f")],
                                 model.selection(), 10, 0)
Union_1 = model.addUnion(Part_1_doc,
                         [model.selection("SOLID", "Extrusion_1_1_1"),
                          model.selection("SOLID", "Extrusion_1_1_2")])
model.do()
model.end()