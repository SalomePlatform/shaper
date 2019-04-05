from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(30, 30, 15)
SketchLine_1 = Sketch_1.addLine(10, 80, 80, 27)
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchCircle_1.results()[1], SketchLine_1.result())
model.do()
model.end()
