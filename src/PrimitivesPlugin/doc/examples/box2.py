from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Point_2 = model.addPoint(Part_1_doc, 20, 30, 40)
Point_3 = model.addPoint(Part_1_doc, 40, 40, 50)
Box_2 = model.addBox(Part_1_doc, model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2"))
model.do()
model.end()
