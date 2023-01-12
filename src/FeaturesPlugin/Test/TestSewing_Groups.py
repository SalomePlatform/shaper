import sys
import salome

salome.salome_init()

from SketchAPI import *
from GeomAPI import GeomAPI_Shape

from salome.shaper import model


model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(60, 0, 0, 0)

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 75)

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(0, 75, 60, 75)

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(60, 75, 60, 0)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
Sketch_1.setHorizontal(SketchLine_1.result())
Sketch_1.setVertical(SketchLine_2.result())
Sketch_1.setHorizontal(SketchLine_3.result())
Sketch_1.setVertical(SketchLine_4.result())
model.do()

### Create Face
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")])

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))

### Create SketchLine
SketchLine_5 = Sketch_2.addLine(75, 0.0001, 0, 0)

### Create SketchProjection
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchPoint_2.result())

### Create SketchLine
SketchLine_6 = Sketch_2.addLine(0, 0, 0, 50)

### Create SketchLine
SketchLine_7 = Sketch_2.addLine(0, 50, 75, 50)

### Create SketchLine
SketchLine_8 = Sketch_2.addLine(75, 50, 75, 0.0001)
Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
Sketch_2.setVertical(SketchLine_6.result())
Sketch_2.setHorizontal(SketchLine_7.result())
Sketch_2.setVertical(SketchLine_8.result())

### Create SketchProjection
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_9 = SketchProjection_3.createdFeature()
Sketch_2.setDistance(SketchLine_5.startPoint(), SketchLine_9.result(), 0.0001, True)

### Create SketchProjection
SketchProjection_4 = Sketch_2.addProjection(model.selection("VERTEX", "Face_1_1/Modified_Vertex&Sketch_1/SketchLine_2_EndVertex&Sketch_1/SketchLine_3_StartVertex"), False)
SketchPoint_3 = SketchProjection_4.createdFeature()
Sketch_2.setHorizontalDistance(SketchLine_8.endPoint(), SketchAPI_Point(SketchPoint_3).coordinates(), 0)
model.do()

### Create Face
Face_2 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_8r-SketchLine_7r-SketchLine_6r-SketchLine_5r")])

### Create Group
Group_1_objects = [model.selection("EDGE", "Face_1_1/Modified_Edge&Sketch_1/SketchLine_4"),
                   model.selection("EDGE", "Face_1_1/Modified_Edge&Sketch_1/SketchLine_1"),
                   model.selection("EDGE", "Face_1_1/Modified_Edge&Sketch_1/SketchLine_3"),
                   model.selection("EDGE", "Face_1_1/Modified_Edge&Sketch_1/SketchLine_2")]
Group_1 = model.addGroup(Part_1_doc, "Edges", Group_1_objects)
model.do()

model.testNbResults(Group_1, 1)
model.testNbSubShapes(Group_1, GeomAPI_Shape.EDGE, [4])

### Create Group
Group_2_objects = [model.selection("EDGE", "Face_2_1/Modified_Edge&Sketch_2/SketchLine_6"),
                   model.selection("EDGE", "Face_2_1/Modified_Edge&Sketch_2/SketchLine_5"),
                   model.selection("EDGE", "Face_2_1/Modified_Edge&Sketch_2/SketchLine_7"),
                   model.selection("EDGE", "Face_2_1/Modified_Edge&Sketch_2/SketchLine_8")]
Group_2 = model.addGroup(Part_1_doc, "Edges", Group_2_objects)
model.do()

model.testNbResults(Group_2, 1)
model.testNbSubShapes(Group_2, GeomAPI_Shape.EDGE, [4])

### Create Sewing
Sewing_1 = model.addSewing(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("FACE", "Face_2_1")], 0.0001, allowNonManifold = True, alwaysCreateResult = False)
model.do()

model.testResults(Sewing_1, 1, [0], [1], [2], [7], [6])

# Move the groups after Sewing (move to the end)
Part_1_doc.moveFeature(Group_1.feature(), Sewing_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Sewing_1.feature())
model.end()

print("-------------------------------------------------------------------------")
print("IMPORTANT NOTE:")
print("The following checks will fail, as long as bos #33216 does not get fixed!")
print("-------------------------------------------------------------------------")
model.testNbResults(Group_1, 1)
model.testNbSubShapes(Group_1, GeomAPI_Shape.EDGE, [4])
model.testNbResults(Group_2, 1)
model.testNbSubShapes(Group_2, GeomAPI_Shape.EDGE, [4])

# Perform same test, but moving groups after Shell feature
model.undo()
model.undo()
model.testNbSubShapes(Group_1, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Group_2, GeomAPI_Shape.EDGE, [4])

### Build Shell
model.begin()
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("FACE", "Face_2_1")])
model.do()

model.testResults(Shell_1, 2, [0,0], [1,1], [1,1], [4,4], [4,4])
model.testNbSubShapes(Shell_1, GeomAPI_Shape.EDGE, [4,4])

# Move the groups after Shell (move to the end)
Part_1_doc.moveFeature(Group_1.feature(), Shell_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Shell_1.feature())
model.end()

model.testNbResults(Group_1, 1)
model.testNbSubShapes(Group_1, GeomAPI_Shape.EDGE, [4])
model.testNbResults(Group_2, 1)
model.testNbSubShapes(Group_2, GeomAPI_Shape.EDGE, [4])
