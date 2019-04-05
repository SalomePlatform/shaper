from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Axis_4 = model.addAxis(Part_1_doc, model.selection("FACE", "Box_1_1/Top"), model.selection("FACE", "Box_1_1/Left"))
model.do()
model.end()
