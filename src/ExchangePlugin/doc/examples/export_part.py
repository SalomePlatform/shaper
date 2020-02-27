from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Export_1 = model.exportPart(Part_1_doc, "~/box.shaperpart", [model.selection("SOLID", "Box_1_1")])
model.end()
