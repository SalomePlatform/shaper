import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-325.90051457976, -18.010291595197, 258.003584159371)
model.do()
Sketch_1.result()[0].setDeflection(5e-006)
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f")], model.selection(), 10, 0)
Extrusion_1.result()[0].setDeflection(0.5)
model.end()

assert(model.checkPythonDump())
