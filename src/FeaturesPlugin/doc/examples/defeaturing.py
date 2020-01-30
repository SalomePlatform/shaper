from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Fillet_1_objects = [model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]")]
Fillet_1 = model.addFillet(Part_1_doc, Fillet_1_objects, 2)
Defeaturing_1 = model.addDefeaturing(Part_1_doc, [model.selection("FACE", "Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_3"), model.selection("FACE", "(Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_1)(Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_2)(Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_3)")])

model.end()
