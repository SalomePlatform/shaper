from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(7, 11, 2)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, 0)
Extrusion_1.result().setTransparency(0.75)
Copy_1 = model.addCopy(Part_1_doc, [model.selection("FACE", "Extrusion_1_1/From_Face"), model.selection("FACE", "Extrusion_1_1/To_Face")], 2)
Copy_1.result().setColor(0, 170, 0)
Copy_1.results()[1].setColor(0, 170, 0)
Copy_1.results()[2].setColor(0, 170, 0)
Copy_1.results()[3].setColor(0, 170, 0)

model.end()
