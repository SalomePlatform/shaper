from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 20, 20, 20)
Chamfer_1 = model.addChamfer(Part_1_doc, [model.selection("FACE", "Box_1_1/Top")], True, 2, 2)
model.do()
model.end()
