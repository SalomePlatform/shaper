from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Group_1_objects = [model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]"),
                   model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"),
                   model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"),
                   model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Left]")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_2_objects = [model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"),
                   model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Bottom]"),
                   model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"),
                   model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Group_3_objects = [model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"),
                   model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]"),
                   model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Top]"),
                   model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]")]
Group_3 = model.addGroup(Part_1_doc, Group_3_objects)
Group_4_objects = [model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Bottom]"),
                   model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Bottom]"),
                   model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Bottom]"),
                   model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]")]
Group_4 = model.addGroup(Part_1_doc, Group_4_objects)
GroupSubstraction_1 = model.addGroupSubstraction(Part_1_doc,
                                                 [model.selection("COMPOUND", "Group_1"),
                                                  model.selection("COMPOUND", "Group_2")],
                                                 [model.selection("COMPOUND", "Group_3"),
                                                  model.selection("COMPOUND", "Group_4")])
model.end()
