from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "Box_1_1/Top"),
                                      model.selection("FACE", "Box_1_1/Left")])
Group_2 = model.addGroup(Part_1_doc, [model.selection("FACE", "Box_1_1/Front"),
                                      model.selection("FACE", "Box_1_1/Top")])
GroupAddition_1 = model.addGroupAddition(Part_1_doc, [model.selection("COMPOUND", "Group_1"),
                                                      model.selection("COMPOUND", "Group_2")])
model.end()
