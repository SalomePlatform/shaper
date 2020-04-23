from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

### Create Wire
Wire_1_objects = [model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]"),
                  model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"),
                  model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]"),
                  model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects, False)

### Create Fillet1D
Fillet1D_1_objects = [model.selection("VERTEX", "[Wire_1_1/Edge_2]e[Wire_1_1/Edge_3]e"),
                      model.selection("VERTEX", "[Wire_1_1/Edge_3]e[Wire_1_1/Edge_4]e")]
Fillet1D_1 = model.addFillet(Part_1_doc, Fillet1D_1_objects, 3)

model.end()
