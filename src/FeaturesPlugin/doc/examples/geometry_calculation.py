import os
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
properties = model.getGeometryCalculation(Part_1_doc,model.selection("SOLID", "Box_1_1"))
print(" length: ", properties[0])
print(" area: ", properties[1])
print(" volume: ", properties[2])
model.end()