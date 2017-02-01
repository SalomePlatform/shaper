from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-143.8658546926619, 0.1897622851492952, 114.0641755357124)
SketchLine_1 = Sketch_1.addLine(-141.3317208711381, 114.2257842537203, -143.8607239569611, -118.2013550640528)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_1.results()[1])
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_2 = Sketch_2.addCircle(-111.0503834053735, -148.4878232655183, 89.21253106334201)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1"), model.selection("COMPOUND", "Sketch_2")], model.selection(), 10, 0)
Boolean_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Extrusion_1_2_2"), model.selection("SOLID", "Extrusion_1_2_1")], [model.selection("SOLID", "Extrusion_1_1")])
model.end()

assert(model.checkPythonDump())
