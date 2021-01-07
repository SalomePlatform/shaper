from salome.shaper import model
import os

model.begin()
file_path = os.path.join(os.getenv("DATA_DIR"), "test.stl")
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Export_1 = model.exportToSTL(Part_1_doc, file_path,
                              model.selection("SOLID", "Box_1_1"),0.0001,0.5,True,False)
model.do()
model.end()
