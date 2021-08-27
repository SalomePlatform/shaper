from salome.shaper import model
import os

model.begin()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
model.do()
    
### Create BoundingBox
Normal_1 = model.getNormal(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))
model.do()

model.end()
