from salome.shaper import model
import os

model.begin()
file_path = os.path.join(os.getenv("DATA_DIR"),"Shapes","Step","black_and_white.step")
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Import_1 = model.addImportSTEP(Part_1_doc,file_path, True, True, True)
model.do()
model.end()
