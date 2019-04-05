from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(27, 25, 5)
SketchArc_1 = Sketch_1.addArc(55.58, 56.8, 70.53, 54.47, 65.76, 67.99, True)
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchCircle_1.results()[1], SketchArc_1.results()[1])
model.do()
model.end()
