from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "dx", '10')
model.addParameter(Part_1_doc, "alfa", '90')

Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], axis = model.selection("EDGE", "PartSet/OX"), distance = "dx", keepSubResults = True)
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Translation_1_1")], axis = model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"), angle = "alfa", keepSubResults = True)

Shell_1_objects = [model.selection("FACE", "Box_1_1/Top"),
                   model.selection("FACE", "Box_1_1/Left"),
                   model.selection("FACE", "Box_1_1/Right"),
                   model.selection("FACE", "Box_1_1/Back"),
                   model.selection("FACE", "Box_1_1/Bottom")]
Shell_1 = model.addShell(Part_1_doc, Shell_1_objects)

Shell_2_objects = [model.selection("FACE", "Rotation_1_1/MF:Rotated&Box_2_1/Top"),
                   model.selection("FACE", "Rotation_1_1/MF:Rotated&Box_2_1/Left"),
                   model.selection("FACE", "Rotation_1_1/MF:Rotated&Box_2_1/Front"),
                   model.selection("FACE", "Rotation_1_1/MF:Rotated&Box_2_1/Right"),
                   model.selection("FACE", "Rotation_1_1/MF:Rotated&Box_2_1/Bottom")]
Shell_2 = model.addShell(Part_1_doc, Shell_2_objects)

Sewing_1 = model.addSewing(Part_1_doc, [model.selection("SHELL", "Shell_2_1"), model.selection("SHELL", "Shell_1_1")], 1e-07, allowNonManifold = False, alwaysCreateResult = True)

model.end()
