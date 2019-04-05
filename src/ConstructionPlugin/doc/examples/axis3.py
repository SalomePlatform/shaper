from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Axis_4 = model.addAxis(Part_1_doc, model.selection("EDGE", "PartSet/OY"))
model.do()
model.end()
