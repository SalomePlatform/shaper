from salome.shaper import model
import os 

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

file_xao = os.path.join(os.getenv("DATA_DIR"), "test.xao")
Export_1 = model.exportToXAO(Part_1_doc, file_xao,
                             model.selection("SOLID", "Box_1_1"), "author", "box")

file_xao = os.path.join(os.getenv("DATA_DIR"), "test1.xao")
file_brep = os.path.join(os.getenv("DATA_DIR"), "test1_shape.brep")
Export_2 = model.exportToXAO(Part_1_doc, file_xao,
                             model.selection("SOLID", "Box_1_1"), "author", "box",
                             file_brep)
model.do()
model.end()
