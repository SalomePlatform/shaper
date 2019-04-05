from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(38.6, 47.5, 31.5)
SketchCircle_2 = Sketch_1.addCircle(0.0, 14.1, 15.0, 0.1, 17.2, 19.9)
model.do()
model.end()
