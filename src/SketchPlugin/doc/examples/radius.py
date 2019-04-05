from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(35, 50, 25)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 25)
model.do()
model.end()