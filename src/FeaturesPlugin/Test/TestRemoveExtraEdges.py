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

from SketchAPI import *
from GeomAPI import GeomAPI_Shape
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

model.addParameter(Part_1_doc, "r_sphere", '15')
model.addParameter(Part_1_doc, "r_cube", '5')
model.addParameter(Part_1_doc, "h_cube", '2.5')

### Create Half-sphere
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), "r_sphere")
Box_1 = model.addBox(Part_1_doc, 0, 0, "-r_sphere", "r_sphere", "r_sphere", "r_sphere")
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Sphere_1_1")], [model.selection("SOLID", "Box_1_1")], keepSubResults = True)

### Create tools to split Half-sphere into 5 parts
Box_2 = model.addBox(Part_1_doc, 0, 0, "h_cube", "r_cube", "r_cube", "h_cube")
Box_2.result().setColor(0, 255, 0)

# Sketch_1
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))

SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "[Box_2_1/Right][Box_2_1/Bottom]"), True)
SketchLine_1 = SketchProjection_1.createdFeature()

SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "[Box_2_1/Back][Box_2_1/Bottom]"), True)
SketchLine_2 = SketchProjection_2.createdFeature()

SketchProjection_3 = Sketch_1.addProjection(model.selection("EDGE", "[Box_2_1/Left][Box_2_1/Bottom]"), True)
SketchLine_3 = SketchProjection_3.createdFeature()

SketchProjection_4 = Sketch_1.addProjection(model.selection("EDGE", "[Box_2_1/Front][Box_2_1/Bottom]"), True)
SketchLine_4 = SketchProjection_4.createdFeature()

SketchLine_5 = Sketch_1.addLine(5, 5, 10.60660171779821, 10.60660171779821)
Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).endPoint(), SketchLine_5.startPoint(), True)

SketchProjection_5 = Sketch_1.addProjection(model.selection("EDGE", "Cut_1_1/Generated_Edge&Box_1_1/Top&Sphere_1_1/Face_1&new_weak_name_2"), False)
SketchArc_1 = SketchProjection_5.createdFeature()
Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchArc_1.results()[1], True)

SketchLine_6 = Sketch_1.addLine(-5, 5, -10.60660171779821, 10.60660171779821)
Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).startPoint(), SketchLine_6.startPoint(), True)
Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchArc_1.results()[1], True)

SketchLine_7 = Sketch_1.addLine(5, -5, 10.60660171779821, -10.60660171779821)
Sketch_1.setCoincident(SketchAPI_Line(SketchLine_3).endPoint(), SketchLine_7.startPoint(), True)
Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchArc_1.results()[1], True)

SketchLine_8 = Sketch_1.addLine(-5, -5, -10.60660171779822, -10.60660171779821)
Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_8.startPoint(), True)
Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchArc_1.results()[1], True)

SketchProjection_6 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_9 = SketchProjection_6.createdFeature()

Sketch_1.setAngle(SketchLine_5.result(), SketchLine_9.result(), 45, type = "Direct", is_active = True)
Sketch_1.setAngle(SketchLine_9.result(), SketchLine_7.result(), 45, type = "Direct", is_active = True)
Sketch_1.setParallel(SketchLine_7.result(), SketchLine_6.result(), True)
Sketch_1.setParallel(SketchLine_8.result(), SketchLine_5.result(), True)

model.do()

# Sketch_2
Plane_1 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/YOZ"), model.selection("EDGE", "PartSet/OZ"), 45)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))

SketchProjection_7 = Sketch_2.addProjection(model.selection("EDGE", "[Box_2_1/Back][Box_2_1/Right]"), True)
SketchLine_10 = SketchProjection_7.createdFeature()

SketchProjection_8 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_6"), True)
SketchLine_11 = SketchProjection_8.createdFeature()

SketchLine_12 = Sketch_2.addLine(-7.071067811865476, -5, -16.6032446479818, -14.53217683611639)
Sketch_2.setCoincident(SketchAPI_Line(SketchLine_10).endPoint(), SketchLine_12.startPoint(), True)

SketchLine_13 = Sketch_2.addLine(-16.6032446479818, -14.53217683611639, -15, 2.081668171172169e-16)
Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint(), True)
Sketch_2.setCoincident(SketchAPI_Line(SketchLine_11).endPoint(), SketchLine_13.endPoint(), True)

Sketch_2.setAngle(SketchLine_11.result(), SketchLine_12.result(), 45, type = "Direct", is_active = True)

model.do()

# Split tool #1
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchProjection_8f-SketchLine_13r-SketchLine_12r-SketchProjection_7r")])
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("FACE", "Face_1_1")], model.selection("EDGE", "PartSet/OZ"), 4, keepSubResults = True)

# Sketch_3
Plane_2 = model.addPlane(Part_1_doc, model.selection("VERTEX", "AngularCopy_1_1_2/MV:Rotated&Sketch_2/SketchLine_12_EndVertex&Sketch_2/SketchLine_13_StartVertex"), model.selection("VERTEX", "AngularCopy_1_1_1/MV:Rotated&Sketch_2/SketchLine_12_EndVertex&Sketch_2/SketchLine_13_StartVertex"), model.selection("VERTEX", "AngularCopy_1_1_1/MV:Rotated&Sketch_2/SketchProjection_7_EndVertex&Sketch_2/SketchLine_12_StartVertex"))
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_2"))

SketchProjection_9 = Sketch_3.addProjection(model.selection("EDGE", "AngularCopy_1_1_1/ME:Rotated&Sketch_2/SketchLine_12"), True)
SketchLine_14 = SketchProjection_9.createdFeature()

SketchProjection_10 = Sketch_3.addProjection(model.selection("EDGE", "AngularCopy_1_1_2/ME:Rotated&Sketch_2/SketchLine_12"), True)
SketchLine_15 = SketchProjection_10.createdFeature()

SketchLine_16 = Sketch_3.addLine(-18.64371894381798, -11.74026688028719, -18.64371894381799, 11.74026688028718)
Sketch_3.setCoincident(SketchAPI_Line(SketchLine_14).endPoint(), SketchLine_16.startPoint(), True)
Sketch_3.setCoincident(SketchAPI_Line(SketchLine_15).endPoint(), SketchLine_16.endPoint(), True)

SketchLine_17 = Sketch_3.addLine(-6.969234250586757, -5.000000000000003, -6.96923425058676, 4.999999999999999)
Sketch_3.setCoincident(SketchAPI_Line(SketchLine_14).startPoint(), SketchLine_17.startPoint(), True)
Sketch_3.setCoincident(SketchAPI_Line(SketchLine_15).startPoint(), SketchLine_17.endPoint(), True)

model.do()

# Split tool #2
Face_2 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_17f-SketchProjection_10f-SketchLine_16r-SketchProjection_9r")])
AngularCopy_2 = model.addMultiRotation(Part_1_doc, [model.selection("FACE", "Face_2_1")], model.selection("EDGE", "PartSet/OZ"), 4, keepSubResults = True)

### Create Split
Split_1_objects_2 = [model.selection("COMPOUND", "AngularCopy_1_1"),
                     model.selection("COMPOUND", "AngularCopy_2_1"),
                     model.selection("SOLID", "Box_2_1")]
Split_1 = model.addSplit(Part_1_doc, [model.selection("SOLID", "Cut_1_1")],
                         Split_1_objects_2, keepSubResults = True)
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

### Create RemoveExtraEdges
RemoveExtraEdges_1 = model.addRemoveExtraEdges(Part_1_doc, model.selection("COMPSOLID", "Remove_SubShapes_1_1"), True)

model.end()

# check result
REE = RemoveExtraEdges_1
model.testNbResults(REE, 1)
model.testNbSubResults(REE, [5])
model.testNbSubShapes(REE, GeomAPI_Shape.SOLID, [5])
model.testNbSubShapes(REE, GeomAPI_Shape.FACE, [30])
model.testNbSubShapes(REE, GeomAPI_Shape.EDGE, [120])
model.testNbSubShapes(REE, GeomAPI_Shape.VERTEX, [240])

assert(model.checkPythonDump())
