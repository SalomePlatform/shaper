from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 20, 10, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], axis = model.selection("EDGE", "PartSet/OX"), distance = 10, keepSubResults = True)
model.do()
GlueFaces_1 = model.addGlueFaces(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Translation_1_1")], 1.0e-7, True)
model.do()
model.end()
