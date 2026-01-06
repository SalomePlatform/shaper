# Copyright (C) 2014-2026  CEA, EDF
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

import sys
import salome

salome.salome_init()

###
### SHAPER component
###

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "r_sphere", '15')
model.addParameter(Part_1_doc, "r_cube", '5')
model.addParameter(Part_1_doc, "h_cube", '2.5')

### Create Sphere
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), "r_sphere")

### Create Rotation
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Sphere_1_1")], axis = model.selection("EDGE", "PartSet/OX"), angle = 90, keepSubResults = True)

### Create Box
Box_1 = model.addBox(Part_1_doc, 0, 0, "-r_sphere", "r_sphere", "r_sphere", "r_sphere")

### Create Cut
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Rotation_1_1")], [model.selection("SOLID", "Box_1_1")], keepSubResults = True)
Cut_1.result().setColor(0, 0, 255)
Cut_1.result().setTransparency(0.5)

### Create Box
Box_2 = model.addBox(Part_1_doc, 0, 0, "h_cube", "r_cube", "r_cube", "h_cube")
Box_2.result().setColor(0, 255, 0)

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "[Box_2_1/Right][Box_2_1/Bottom]"), True)
SketchLine_1 = SketchProjection_1.createdFeature()

### Create SketchProjection
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "[Box_2_1/Back][Box_2_1/Bottom]"), True)
SketchLine_2 = SketchProjection_2.createdFeature()

### Create SketchProjection
SketchProjection_3 = Sketch_1.addProjection(model.selection("EDGE", "[Box_2_1/Left][Box_2_1/Bottom]"), True)
SketchLine_3 = SketchProjection_3.createdFeature()

### Create SketchProjection
SketchProjection_4 = Sketch_1.addProjection(model.selection("EDGE", "[Box_2_1/Front][Box_2_1/Bottom]"), True)
SketchLine_4 = SketchProjection_4.createdFeature()

### Create SketchLine
SketchLine_5 = Sketch_1.addLine(5, 5, 10.60660171779821, 10.60660171779821)
Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).endPoint(), SketchLine_5.startPoint())

### Create SketchProjection
SketchProjection_5 = Sketch_1.addProjection(model.selection("EDGE", "[Cut_1_1/Modified_Face&Sphere_1_1/Face_1][Cut_1_1/Modified_Face&Box_1_1/Top][new_weak_name_4]"), False)
SketchArc_1 = SketchProjection_5.createdFeature()
Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchArc_1.results()[1])

### Create SketchLine
SketchLine_6 = Sketch_1.addLine(-5, 5, -10.60660171779821, 10.60660171779821)
Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).startPoint(), SketchLine_6.startPoint())

### Create SketchProjection
SketchProjection_6 = Sketch_1.addProjection(model.selection("EDGE", "Cut_1_1/Generated_Edge&Box_1_1/Top&Sphere_1_1/Face_1&new_weak_name_2"), False)
SketchArc_2 = SketchProjection_6.createdFeature()
Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchArc_2.results()[1])

### Create SketchLine
SketchLine_7 = Sketch_1.addLine(5, -5, 10.60660171779821, -10.60660171779821)
Sketch_1.setCoincident(SketchAPI_Line(SketchLine_3).endPoint(), SketchLine_7.startPoint())

### Create SketchProjection
SketchProjection_7 = Sketch_1.addProjection(model.selection("EDGE", "[Cut_1_1/Modified_Face&Sphere_1_1/Face_1][Cut_1_1/Modified_Face&Box_1_1/Top][new_weak_name_3]"), False)
SketchArc_3 = SketchProjection_7.createdFeature()
Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchArc_3.results()[1])

### Create SketchLine
SketchLine_8 = Sketch_1.addLine(-5, -5, -10.60660171779822, -10.60660171779821)
Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_8.startPoint())

### Create SketchProjection
SketchProjection_8 = Sketch_1.addProjection(model.selection("EDGE", "Cut_1_1/Generated_Edge&Box_1_1/Top&Sphere_1_1/Face_1&new_weak_name_1"), False)
SketchArc_4 = SketchProjection_8.createdFeature()
Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchArc_4.results()[1])

### Create SketchProjection
SketchProjection_9 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_9 = SketchProjection_9.createdFeature()

### Create SketchConstraintAngle
Sketch_1.setAngle(SketchLine_5.result(), SketchLine_9.result(), 45, type = "Direct")

### Create SketchConstraintAngle
Sketch_1.setAngle(SketchLine_9.result(), SketchLine_7.result(), 45, type = "Direct")
Sketch_1.setParallel(SketchLine_7.result(), SketchLine_6.result())
Sketch_1.setParallel(SketchLine_8.result(), SketchLine_5.result())
model.do()

### Create Plane
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/YOZ"), model.selection("EDGE", "PartSet/OZ"), 45)

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))

### Create SketchProjection
SketchProjection_10 = Sketch_2.addProjection(model.selection("EDGE", "[Box_2_1/Back][Box_2_1/Right]"), True)
SketchLine_10 = SketchProjection_10.createdFeature()

### Create SketchProjection
SketchProjection_11 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_6"), True)
SketchLine_11 = SketchProjection_11.createdFeature()

### Create SketchLine
SketchLine_12 = Sketch_2.addLine(-7.071067811865476, -5, -16.6032446479818, -14.53217683611639)
Sketch_2.setCoincident(SketchAPI_Line(SketchLine_10).endPoint(), SketchLine_12.startPoint())

### Create SketchLine
SketchLine_13 = Sketch_2.addLine(-16.6032446479818, -14.53217683611639, -15, 2.081668171172169e-16)
Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
Sketch_2.setCoincident(SketchAPI_Line(SketchLine_11).endPoint(), SketchLine_13.endPoint())

### Create SketchConstraintAngle
Sketch_2.setAngle(SketchLine_11.result(), SketchLine_12.result(), 45, type = "Direct")
model.do()

### Create Face
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchProjection_11f-SketchLine_13r-SketchLine_12r-SketchProjection_10r")])

### Create AngularCopy
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("FACE", "Face_1_1")], model.selection("EDGE", "PartSet/OZ"), 4, keepSubResults = True)

### Create Plane
Plane_5 = model.addPlane(Part_1_doc, model.selection("VERTEX", "AngularCopy_1_1_2/MV:Rotated&Sketch_2/SketchLine_12_EndVertex&Sketch_2/SketchLine_13_StartVertex"), model.selection("VERTEX", "AngularCopy_1_1_1/MV:Rotated&Sketch_2/SketchLine_12_EndVertex&Sketch_2/SketchLine_13_StartVertex"), model.selection("VERTEX", "AngularCopy_1_1_1/MV:Rotated&Sketch_2/SketchProjection_10_EndVertex&Sketch_2/SketchLine_12_StartVertex"))

### Create Sketch
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_2"))

### Create SketchProjection
SketchProjection_12 = Sketch_3.addProjection(model.selection("EDGE", "AngularCopy_1_1_1/ME:Rotated&Sketch_2/SketchLine_12"), True)
SketchLine_14 = SketchProjection_12.createdFeature()

### Create SketchProjection
SketchProjection_13 = Sketch_3.addProjection(model.selection("EDGE", "AngularCopy_1_1_2/ME:Rotated&Sketch_2/SketchLine_12"), True)
SketchLine_15 = SketchProjection_13.createdFeature()

### Create SketchLine
SketchLine_16 = Sketch_3.addLine(-18.64371894381798, -11.74026688028719, -18.64371894381799, 11.74026688028718)
Sketch_3.setCoincident(SketchAPI_Line(SketchLine_14).endPoint(), SketchLine_16.startPoint())
Sketch_3.setCoincident(SketchAPI_Line(SketchLine_15).endPoint(), SketchLine_16.endPoint())

### Create SketchLine
SketchLine_17 = Sketch_3.addLine(-6.969234250586757, -5.000000000000003, -6.96923425058676, 4.999999999999999)
Sketch_3.setCoincident(SketchAPI_Line(SketchLine_14).startPoint(), SketchLine_17.startPoint())
Sketch_3.setCoincident(SketchAPI_Line(SketchLine_15).startPoint(), SketchLine_17.endPoint())
model.do()

### Create Face
Face_2 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_17f-SketchProjection_13f-SketchLine_16r-SketchProjection_12r")])

### Create AngularCopy
AngularCopy_2 = model.addMultiRotation(Part_1_doc, [model.selection("FACE", "Face_2_1")], model.selection("EDGE", "PartSet/OZ"), 4, keepSubResults = True)

### Create Split
Split_1_objects_2 = [model.selection("COMPOUND", "AngularCopy_1_1"),
                     model.selection("COMPOUND", "AngularCopy_2_1"),
                     model.selection("SOLID", "Box_2_1")]
Split_1 = model.addSplit(Part_1_doc, [model.selection("SOLID", "Cut_1_1")], Split_1_objects_2, keepSubResults = True)
Split_1.result().setTransparency(0.49)
Split_1.result().subResult(0).setTransparency(0.49)
Split_1.result().subResult(1).setTransparency(0.49)
Split_1.result().subResult(2).setTransparency(0.49)
Split_1.result().subResult(3).setTransparency(0.49)
Split_1.result().subResult(4).setTransparency(0.49)
Split_1.result().subResult(5).setTransparency(0.49)

### Create Remove_SubShapes
Remove_SubShapes_1 = model.addRemoveSubShapes(Part_1_doc, model.selection("COMPSOLID", "Split_1_1"))
Remove_SubShapes_1.setSubShapesToRemove([model.selection("SOLID", "Split_1_1_5")])

### Create Group
Group_1_objects = [model.selection("FACE", "Remove_SubShapes_1_1_2/Modified_Face&Sphere_1_1/Face_1"),
                   model.selection("FACE", "Remove_SubShapes_1_1_5/Modified_Face&Sphere_1_1/Face_1"),
                   model.selection("FACE", "Remove_SubShapes_1_1_4/Modified_Face&Sphere_1_1/Face_1"),
                   model.selection("FACE", "Remove_SubShapes_1_1_1/Modified_Face&Sphere_1_1/Face_1"),
                   model.selection("FACE", "Remove_SubShapes_1_1_3/Modified_Face&Sphere_1_1/Face_1")]
Group_1 = model.addGroup(Part_1_doc, "Faces", Group_1_objects)
Group_1.setName("Sphere")
Group_1.result().setName("Sphere")

### Create Group
Group_2_objects = [model.selection("FACE", "Remove_SubShapes_1_1_1/Modified_Face&Box_1_1/Top"),
                   model.selection("FACE", "Remove_SubShapes_1_1_2/Modified_Face&Box_1_1/Top"),
                   model.selection("FACE", "Remove_SubShapes_1_1_5/Modified_Face&Box_1_1/Top"),
                   model.selection("FACE", "Remove_SubShapes_1_1_3/Modified_Face&Box_1_1/Top")]
Group_2 = model.addGroup(Part_1_doc, "Faces", Group_2_objects)
Group_2.setName("Sym")
Group_2.result().setName("Sym")

### Create Group
Group_3_objects = [model.selection("FACE", "Remove_SubShapes_1_1_3/Modified_Face&Box_2_1/Right"),
                   model.selection("FACE", "Remove_SubShapes_1_1_5/Modified_Face&Box_2_1/Front"),
                   model.selection("FACE", "Remove_SubShapes_1_1_2/Modified_Face&Box_2_1/Left"),
                   model.selection("FACE", "Remove_SubShapes_1_1_1/Modified_Face&Box_2_1/Back")]
Group_3 = model.addGroup(Part_1_doc, "Faces", Group_3_objects)
Group_3.setName("Cube")
Group_3.result().setName("Cube")

### Create Group
Group_4_objects = [model.selection("SOLID", "Remove_SubShapes_1_1_2"),
                   model.selection("SOLID", "Remove_SubShapes_1_1_1"),
                   model.selection("SOLID", "Remove_SubShapes_1_1_3"),
                   model.selection("SOLID", "Remove_SubShapes_1_1_5"),
                   model.selection("SOLID", "Remove_SubShapes_1_1_4")]
Group_4 = model.addGroup(Part_1_doc, "Solids", Group_4_objects)
Group_4.setName("Fluide")
Group_4.result().setName("Fluide")

### Create Group
filter_1 = model.addFilter(name = "OppositeToEdge", args = [model.selection("EDGE", "Remove_SubShapes_1_1_1/Modified_Edge&Sketch_2/SketchProjection_11&new_weak_name_1")])

filters = model.filters(Part_1_doc, [filter_1])

nEdges = len(filters.select("EDGE"))
assert nEdges==8, "Wrong number of filtered edges in filter: {}, expected 8".format(nEdges)

### Select all suitable edges
selected_edges = filters.select("Edges")

Group_5_objects = selected_edges + (filters,)
Group_5 = model.addGroup(Part_1_doc, "Edges", Group_5_objects)
Group_5.setName("edges_radial")
Group_5.result().setName("edges_radial")

model.end()
000
0