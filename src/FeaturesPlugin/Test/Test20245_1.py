# Copyright (C) 2020-2021  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "r_ext", "80")
model.addParameter(Part_1_doc, "r_int", "60")
model.addParameter(Part_1_doc, "r_cut", "30")
model.addParameter(Part_1_doc, "h", "200")

### Create Cylinder
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "r_ext", "h")

### Create Cylinder
Cylinder_2 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "r_int", "h")

### Create Sphere
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), "r_ext")

### Create Sphere
Sphere_2 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), "r_int")

### Create Rotation
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("COMPOUND", "all-in-Sphere_1")], axis = model.selection("EDGE", "PartSet/OY"), angle = 90, keepSubResults = True)

### Create Rotation
Rotation_2 = model.addRotation(Part_1_doc, [model.selection("COMPOUND", "all-in-Sphere_2")], axis = model.selection("EDGE", "PartSet/OY"), angle = 90, keepSubResults = True)

### Create Translation
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "Rotation_1_1"), model.selection("COMPOUND", "Rotation_2_1")], axis = model.selection("EDGE", "PartSet/OZ"), distance = "h", keepSubResults = True)

### Create Fuse
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Translation_1_1_1"), model.selection("SOLID", "Cylinder_1_1")], keepSubResults = True)

### Create Fuse
Fuse_2 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Cylinder_2_1"), model.selection("SOLID", "Translation_1_2_1")], keepSubResults = True)

### Create Partition
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Fuse_1_1"), model.selection("SOLID", "Fuse_2_1")], keepSubResults = True)

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(0, 30, -30, 0)

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_2.result())

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(-30, 0, 0, -30)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(0, -30, 30, 0)
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())

### Create SketchProjection
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_5 = SketchProjection_2.createdFeature()
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.result())

### Create SketchLine
SketchLine_6 = Sketch_1.addLine(30, 0, 0, 30)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_6.startPoint())
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_6.endPoint())

### Create SketchLine
SketchLine_7 = Sketch_1.addLine(-30, 0, -110, 0)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_7.startPoint())
Sketch_1.setHorizontal(SketchLine_7.result())

### Create SketchLine
SketchLine_8 = Sketch_1.addLine(0, -30, 0, -110)
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_8.startPoint())
Sketch_1.setVertical(SketchLine_8.result())

### Create SketchLine
SketchLine_9 = Sketch_1.addLine(0, 30, 0, 110)
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_9.startPoint())
Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_2.result())

### Create SketchLine
SketchLine_10 = Sketch_1.addLine(30, 0, 110, 0)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_10.startPoint())
Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_5.result())
Sketch_1.setEqual(SketchLine_10.result(), SketchLine_7.result())
Sketch_1.setEqual(SketchLine_9.result(), SketchLine_8.result())
Sketch_1.setParallel(SketchLine_1.result(), SketchLine_4.result())
Sketch_1.setParallel(SketchLine_6.result(), SketchLine_3.result())
Sketch_1.setCoincident(SketchLine_8.startPoint(), SketchLine_2.result())
Sketch_1.setPerpendicular(SketchLine_1.result(), SketchLine_3.result())
Sketch_1.setLength(SketchLine_7.result(), "r_ext")
Sketch_1.setDistance(SketchLine_7.startPoint(), SketchLine_2.result(), "r_int/2", True)
Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchLine_5.result())
Sketch_1.setEqual(SketchLine_9.result(), SketchLine_10.result())
model.do()

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), "h", 0, "Edges")

### Create Plane
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), "h", False)

### Create Split
Split_1 = model.addSplit(Part_1_doc, [model.selection("COMPSOLID", "Partition_1_1")], [model.selection("FACE", "Plane_1"), model.selection("COMPOUND", "Extrusion_1_1")], keepSubResults = True)

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))

### Create SketchProjection
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "Split_1_1_10/Modified_Edge&Extrusion_1_1_1/To_Edge"), True)
SketchLine_11 = SketchProjection_3.createdFeature()

### Create SketchProjection
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "Split_1_1_9/Modified_Edge&Extrusion_1_1_4/To_Edge"), True)
SketchLine_12 = SketchProjection_4.createdFeature()

### Create SketchProjection
SketchProjection_5 = Sketch_2.addProjection(model.selection("EDGE", "Split_1_1_8/Modified_Edge&Extrusion_1_1_3/To_Edge"), True)
SketchLine_13 = SketchProjection_5.createdFeature()

### Create SketchProjection
SketchProjection_6 = Sketch_2.addProjection(model.selection("EDGE", "Split_1_1_7/Modified_Edge&Extrusion_1_1_2/To_Edge"), True)
SketchLine_14 = SketchProjection_6.createdFeature()
model.do()

### Create Extrusion
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), "r_cut/sqrt(2)", 0, "Faces|Wires")

### Create Sketch
Sketch_3 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))

### Create SketchProjection
SketchProjection_7 = Sketch_3.addProjection(model.selection("EDGE", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchProjection_6][Extrusion_2_1/Generated_Face&Sketch_2/SketchProjection_5]"), True)
SketchLine_15 = SketchProjection_7.createdFeature()

### Create SketchProjection
SketchProjection_8 = Sketch_3.addProjection(model.selection("EDGE", "Split_1_1_7/Modified_Edge&Extrusion_1_1_6/To_Edge"), True)
SketchLine_16 = SketchProjection_8.createdFeature()

### Create SketchProjection
SketchProjection_9 = Sketch_3.addProjection(model.selection("EDGE", "Split_1_1_2/Modified_Edge&Extrusion_1_1_6/To_Edge"), True)
SketchLine_17 = SketchProjection_9.createdFeature()

### Create SketchLine
SketchLine_18 = Sketch_3.addLine(-30, 221.2132034355964, -136.4686528504195, 327.681856286016)
Sketch_3.setCoincident(SketchAPI_Line(SketchLine_15).endPoint(), SketchLine_18.startPoint())

### Create SketchLine
SketchLine_19 = Sketch_3.addLine(-136.4686528504195, 327.681856286016, -80, 200)
Sketch_3.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
Sketch_3.setCoincident(SketchAPI_Line(SketchLine_17).endPoint(), SketchLine_19.endPoint())

### Create SketchConstraintAngle
Sketch_3.setAngle(SketchLine_15.result(), SketchLine_18.result(), 135, type = "Direct")
model.do()

### Create Face
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchProjection_7r-SketchLine_18f-SketchLine_19f-SketchProjection_9r-SketchProjection_8r")])

### Create AngularCopy
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("FACE", "Face_1_1")], model.selection("EDGE", "PartSet/OZ"), 4, keepSubResults = True)

### Create Plane
Plane_5 = model.addPlane(Part_1_doc, model.selection("VERTEX", "AngularCopy_1_1_3/MV:Rotated&Sketch_3/SketchLine_19_StartVertex&Sketch_3/SketchLine_18_EndVertex"), model.selection("VERTEX", "AngularCopy_1_1_4/MV:Rotated&Sketch_3/SketchLine_19_StartVertex&Sketch_3/SketchLine_18_EndVertex"), model.selection("VERTEX", "AngularCopy_1_1_4/MV:Rotated&Sketch_3/SketchLine_18_StartVertex&Sketch_3/SketchProjection_7_EndVertex"))

### Create Sketch
Sketch_4 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_2"))

### Create SketchProjection
SketchProjection_10 = Sketch_4.addProjection(model.selection("EDGE", "AngularCopy_1_1_3/ME:Rotated&Sketch_3/SketchLine_18"), True)
SketchLine_20 = SketchProjection_10.createdFeature()

### Create SketchProjection
SketchProjection_11 = Sketch_4.addProjection(model.selection("EDGE", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchProjection_3][Extrusion_2_1/To_Face]"), True)
SketchLine_21 = SketchProjection_11.createdFeature()

### Create SketchProjection
SketchProjection_12 = Sketch_4.addProjection(model.selection("EDGE", "AngularCopy_1_1_4/ME:Rotated&Sketch_3/SketchLine_18"), True)
SketchLine_22 = SketchProjection_12.createdFeature()

### Create SketchLine
SketchLine_23 = Sketch_4.addLine(-245.2017461009716, 231.7060626516377, -78.06246341670106, 328.2039725015621)
Sketch_4.setCoincident(SketchAPI_Line(SketchLine_22).endPoint(), SketchLine_23.startPoint())
Sketch_4.setCoincident(SketchAPI_Line(SketchLine_20).endPoint(), SketchLine_23.endPoint())
model.do()

### Create Face
Face_2 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_4/Face-SketchProjection_10r-SketchLine_23r-SketchProjection_12r-SketchProjection_11r")])

### Create AngularCopy
AngularCopy_2 = model.addMultiRotation(Part_1_doc, [model.selection("FACE", "Face_2_1")], model.selection("EDGE", "PartSet/OZ"), 4, keepSubResults = True)

### Create Split
Split_2_objects_2 = [model.selection("COMPOUND", "AngularCopy_1_1"),
                     model.selection("COMPOUND", "AngularCopy_2_1"),
                     model.selection("SOLID", "Extrusion_2_1")]
Split_2 = model.addSplit(Part_1_doc, [model.selection("COMPSOLID", "Split_1_1")], Split_2_objects_2, keepSubResults = True)

### Create Group
Group_1_objects = [model.selection("FACE", "Split_2_1_16/Modified_Face&Cylinder_2_1/Face_1"),
                   model.selection("FACE", "Split_2_1_17/Modified_Face&Cylinder_2_1/Face_1"),
                   model.selection("FACE", "Split_2_1_18/Modified_Face&Cylinder_2_1/Face_1"),
                   model.selection("FACE", "Split_2_1_19/Modified_Face&Cylinder_2_1/Face_1")]
Group_1 = model.addGroup(Part_1_doc, "Faces", Group_1_objects)
Group_1.setName("shared_face_cyl")
Group_1.result().setName("shared_face_cyl")

model.end()

from GeomAPI import *

model.testNbResults(Split_2, 1)
model.testNbSubResults(Split_2, [20])
model.testNbSubShapes(Split_2, GeomAPI_Shape.SOLID, [20])
model.testNbSubShapes(Split_2, GeomAPI_Shape.FACE, [120])
model.testNbSubShapes(Split_2, GeomAPI_Shape.EDGE, [492])
model.testNbSubShapes(Split_2, GeomAPI_Shape.VERTEX, [984])
model.testResultsVolumes(Split_2, [5093568.889])

assert(model.checkPythonDump())
