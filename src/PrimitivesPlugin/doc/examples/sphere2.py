from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sphere_1 = model.addSphere(Part_1_doc,5,10,180,245,0,90)
model.do()
model.end()
