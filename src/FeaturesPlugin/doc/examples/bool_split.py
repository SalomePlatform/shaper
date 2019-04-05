from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Cylinder_1 = model.addCylinder(Part_1_doc,
                               model.selection("VERTEX", "PartSet/Origin"),
                               model.selection("EDGE", "PartSet/OZ"), 5, 10)
Split_1 = model.addSplit(Part_1_doc,
                         [model.selection("SOLID", "Cylinder_1_1")],
                         [model.selection("SOLID", "Box_1_1")])
model.do()
model.end()
