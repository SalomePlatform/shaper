from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Ellipsoid_1 = model.addEllipsoid(Part_1_doc, 10, 20, 40)
model.do()
model.end()
