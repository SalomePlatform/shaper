from salome.shaper import model
import os

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
### Create Point
Point_2 = model.addPoint(Part_1_doc, 20, 10, 10)
### Create Box
Box_2 = model.addBox(Part_1_doc, model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Bottom]"), model.selection("VERTEX", "Point_1"))
### Create CompSolid
CompSolid_1 = model.addCompSolid(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Box_2_1")])

### Create Shared_faces
Shared_faces_1 = model.getSharedFaces(Part_1_doc, model.selection("COMPSOLID", "CompSolid_1_1"), "mygroup")

model.do()
model.end()
