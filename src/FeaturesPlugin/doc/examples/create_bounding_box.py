from salome.shaper import model
import os

model.begin()
file_path = os.path.join(os.getenv("DATA_DIR"),"Shapes","Step","screw.step")
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Import_1 = model.addImport(Part_1_doc,file_path)
model.do()
### Create BoundingBox
BoundingBox_1 = model.getBoundingBox(Part_1_doc, model.selection("SOLID", "screw_1"))
model.end()
