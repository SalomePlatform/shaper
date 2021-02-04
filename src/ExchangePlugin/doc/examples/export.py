from salome.shaper import model
import os 

model.begin()
file_path = os.path.join(os.getenv("DATA_DIR"), "test.step")
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Export_1 = model.exportToFile(Part_1_doc, file_path,
                              [model.selection("SOLID", "Box_1_1")])

file_xao = os.path.join(os.getenv("DATA_DIR"), "test.xao")
Export_2 = model.exportToXAO(Part_1_doc, file_xao,
                             model.selection("SOLID", "Box_1_1"), "author", "box")
model.do()
model.end()
