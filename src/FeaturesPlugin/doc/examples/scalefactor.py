from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Scale_1 = model.addScale(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")] , model.selection("VERTEX", "PartSet/Origin"), 2)
model.do()
model.end()
