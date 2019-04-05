from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
CompSolid_1 = model.addCompSolid(Part_1_doc, [model.selection("SOLID", "Box_1_1")])
model.do()
model.end()
