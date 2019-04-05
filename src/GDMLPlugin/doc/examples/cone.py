from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
ConeSegment_1 = model.addConeSegment(Part_1_doc, 7, 11, 5, 8, 12, 0 , 270)
model.do()
model.end()
