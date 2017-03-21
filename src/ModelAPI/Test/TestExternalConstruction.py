# The test for correct referencing for Part feature of whole construction result created in PartSet

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(-104, 73, 102, 265)
SketchLine_2 = Sketch_1.addLine(102, 265, 192, -1)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(192, -1, -104, 73)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
model.do()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", "PartSet/Sketch_1")], model.selection("EDGE", "PartSet/Sketch_1/Edge-SketchLine_3"), 360, 0)
model.end()

assert(model.checkPythonDump())
