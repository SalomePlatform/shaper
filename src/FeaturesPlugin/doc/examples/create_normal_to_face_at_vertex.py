
from salome.shaper import model
import os

model.begin()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
model.do()
    
Normal_1 = model.getNormal(Part_1_doc, model.selection("FACE", "Box_1_1/Front"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Bottom]"))
model.do()

model.end()
