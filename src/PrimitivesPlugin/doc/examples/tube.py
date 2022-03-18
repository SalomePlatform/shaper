from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Torus_1 = model.addTube(Part_1_doc, 3, 5, 12)
model.do()
model.end()
