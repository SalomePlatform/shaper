from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 20, 20, 20)
Chamfer_1_objects = [model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]")]
Chamfer_1 = model.addChamfer(Part_1_doc, Chamfer_1_objects, False, 2, 25)
model.do()
model.end()
