from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cone_1 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10, 5, 10)
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("COMPOUND", "all-in-Cone_1")], model.selection("EDGE", "PartSet/OX"), 120)
model.do()
model.end()
