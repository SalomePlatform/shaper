from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(14.5, 16.0, 71.0, 62.0)
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
model.do()
model.end()