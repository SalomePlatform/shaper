# Copyright (C) 2014-2024  CEA, EDF
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
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Parameter_R = model.addParameter(Part_1_doc, "R", "40")
model.addParameter(Part_1_doc, "h", "3")
model.addParameter(Part_1_doc, "R2", "33.5")
model.addParameter(Part_1_doc, "h2", "4")
model.addParameter(Part_1_doc, "R3", "25.9")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(0, 0, 40)
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchPoint_1.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], "R")
SketchLine_1 = Sketch_1.addLine(0, 0, -20.88093073029438, 34.1172497695333)
SketchLine_1.setAuxiliary(True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_1.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchCircle_1.results()[1])
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), "h", 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_1"))
SketchLine_2 = SketchProjection_1.createdFeature()
SketchCircle_2 = Sketch_2.addCircle(0, 0, 33.5)
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_2.results()[1], "R2")
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchCircle_2.center())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_14")
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2f")], model.selection(), "h2", 0)
Extrusion_3 = model.addExtrusion(Part_1_doc, [], model.selection(), "h2*3", 0)
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_2_1/To_Face"))
SketchCircle_3 = Sketch_3.addCircle(0, 0, 28.25)
SketchConstraintRadius_3 = Sketch_3.setRadius(SketchCircle_3.results()[1], 28.25)
SketchProjection_2 = Sketch_3.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_1"))
SketchLine_3 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_6 = Sketch_3.setCoincident(SketchCircle_3.center(), SketchAPI_Line(SketchLine_3).startPoint())
Extrusion_3.setNestedSketch(Sketch_3)
Extrusion_4 = model.addExtrusion(Part_1_doc, [], model.selection(), "h2", 0)
Sketch_4 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_3_1/To_Face"))
SketchProjection_3 = Sketch_4.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_1"))
SketchLine_4 = SketchProjection_3.createdFeature()
SketchCircle_4 = Sketch_4.addCircle(0, 0, 25.9)
SketchConstraintRadius_4 = Sketch_4.setRadius(SketchCircle_4.results()[1], "R3")
SketchConstraintCoincidence_7 = Sketch_4.setCoincident(SketchAPI_Line(SketchLine_4).startPoint(), SketchCircle_4.center())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_15")
Extrusion_4.setNestedSketch(Sketch_4)
Extrusion_5 = model.addExtrusion(Part_1_doc, [], model.selection(), "h2", 0)
Sketch_5 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_4_1/To_Face"))
SketchProjection_4 = Sketch_5.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_1"))
SketchLine_5 = SketchProjection_4.createdFeature()
SketchCircle_5 = Sketch_5.addCircle(0, 0, 8)
SketchConstraintRadius_5 = Sketch_5.setRadius(SketchCircle_5.results()[1], 8)
SketchConstraintCoincidence_8 = Sketch_5.setCoincident(SketchAPI_Line(SketchLine_5).startPoint(), SketchCircle_5.center())
SketchConstraintCoincidence_8.setName("SketchConstraintCoincidence_16")
Extrusion_5.setNestedSketch(Sketch_5)
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Extrusion_4_1"), model.selection("SOLID", "Extrusion_5_1"), model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_2_1"), model.selection("SOLID", "Extrusion_3_1")], [])
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), model.selection("FACE", "Extrusion_5_1/To_Face"), 0, model.selection(), 0, [model.selection("SOLID", "Fuse_1_1")])
Sketch_6 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/From_Face"))
SketchCircle_6 = Sketch_6.addCircle(0, 0, 4.2)
SketchConstraintRadius_6 = Sketch_6.setRadius(SketchCircle_6.results()[1], 4.2)
SketchPoint_2 = Sketch_6.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintCoincidence_9 = Sketch_6.setCoincident(SketchPoint_2.coordinates(), SketchCircle_6.center())
SketchConstraintCoincidence_9.setName("SketchConstraintCoincidence_17")
ExtrusionCut_1.setNestedSketch(Sketch_6)
Sketch_7 = model.addSketch(Part_1_doc, model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/From_Face"))
SketchCircle_7 = Sketch_7.addCircle(0, -85, 66)
SketchLine_6 = Sketch_7.addLine(model.selection("EDGE", "PartSet/OY"))
SketchConstraintCoincidence_10 = Sketch_7.setCoincident(SketchCircle_7.center(), SketchLine_6.result())
SketchConstraintRadius_7 = Sketch_7.setRadius(SketchCircle_7.results()[1], 66)
SketchConstraintDistance_1 = Sketch_7.setDistance(SketchCircle_7.center(), SketchLine_6.startPoint(), 85)
SketchMultiRotation_1 = Sketch_7.addRotation([SketchCircle_7.results()[1]], SketchLine_6.startPoint(), 120, 3)
[SketchCircle_8, SketchCircle_9] = SketchMultiRotation_1.rotated()
model.do()
ExtrusionCut_2 = model.addExtrusionCut(Part_1_doc, [model.selection("COMPOUND", "Sketch_7")], model.selection(), model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Extrusion_5_1/To_Face"), 0, model.selection(), 0, [model.selection("SOLID", "ExtrusionCut_1_1")])
Sketch_8 = model.addSketch(Part_1_doc, model.selection("FACE", "ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/From_Face"))
SketchLine_7 = Sketch_8.addLine(-2.320957096353877e-016, 11.00000001704673, -7, 11.00000001704673)
SketchConstraintHorizontal_1 = Sketch_8.setHorizontal(SketchLine_7.result())
SketchLine_8 = Sketch_8.addLine(-7, 11.00000001704673, -6.329882773485103e-016, 30.00000001704673)
SketchConstraintCoincidence_11 = Sketch_8.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_8.addLine(model.selection("EDGE", "PartSet/OY"))
SketchConstraintCoincidence_12 = Sketch_8.setCoincident(SketchLine_7.startPoint(), SketchLine_9.result())
SketchConstraintCoincidence_13 = Sketch_8.setCoincident(SketchLine_8.endPoint(), SketchLine_9.result())
SketchConstraintMirror_1 = Sketch_8.addMirror(SketchLine_9.result(), [SketchLine_7.result(), SketchLine_8.result()])
[SketchLine_10, SketchLine_11] = SketchConstraintMirror_1.mirrored()
SketchConstraintLength_1 = Sketch_8.setLength(SketchLine_7.result(), 7)
SketchConstraintDistance_2 = Sketch_8.setDistance(SketchLine_9.startPoint(), SketchLine_7.result(), 11)
SketchConstraintDistance_3 = Sketch_8.setDistance(SketchLine_8.endPoint(), SketchLine_9.startPoint(), 30)
SketchMultiRotation_2_objects = [SketchLine_11.result(), SketchLine_10.result(), SketchLine_8.result(), SketchLine_7.result()]
SketchMultiRotation_2 = Sketch_8.addRotation(SketchMultiRotation_2_objects, SketchLine_9.startPoint(), 120, 3)
[SketchLine_12, SketchLine_13, SketchLine_14, SketchLine_15, SketchLine_16, SketchLine_17, SketchLine_18, SketchLine_19] = SketchMultiRotation_2.rotatedList()
SketchLine_18.setName("SketchLine_15")
SketchLine_18.result().setName("SketchLine_15")
SketchLine_17.setName("SketchLine_18")
SketchLine_17.result().setName("SketchLine_18")
SketchLine_16.setName("SketchLine_14")
SketchLine_16.result().setName("SketchLine_14")
SketchLine_15.setName("SketchLine_17")
SketchLine_15.result().setName("SketchLine_17")
SketchLine_14.setName("SketchLine_13")
SketchLine_14.result().setName("SketchLine_13")
SketchLine_13.setName("SketchLine_16")
SketchLine_13.result().setName("SketchLine_16")
model.do()
ExtrusionCut_3_objects_1 = [model.selection("FACE", "Sketch_8/Face-SketchLine_10f-SketchLine_11f-SketchLine_8r-SketchLine_7r"), model.selection("FACE", "Sketch_8/Face-SketchLine_16f-SketchLine_18r-SketchLine_19r-SketchLine_17f"), model.selection("FACE", "Sketch_8/Face-SketchLine_12f-SketchLine_14r-SketchLine_15r-SketchLine_13f")]
ExtrusionCut_3 = model.addExtrusionCut(Part_1_doc, ExtrusionCut_3_objects_1, model.selection(), model.selection("FACE", "ExtrusionCut_2_1/Modified_Face&Extrusion_4_1/To_Face"), 0, model.selection(), 0, [model.selection("SOLID", "ExtrusionCut_2_1")])

# Test reexecution after parameter change
Parameter_R.setValue(50)
model.do()
model.testResultsVolumes(ExtrusionCut_3, [35832.402050074902945198118686676])
Parameter_R.setValue(40)
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(ExtrusionCut_3, 1)
model.testNbSubResults(ExtrusionCut_3, [0])
model.testNbSubShapes(ExtrusionCut_3, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_3, GeomAPI_Shape.FACE, [50])
model.testNbSubShapes(ExtrusionCut_3, GeomAPI_Shape.EDGE, [300])
model.testNbSubShapes(ExtrusionCut_3, GeomAPI_Shape.VERTEX, [600])
model.testResultsVolumes(ExtrusionCut_3, [34439.077343526856566313654184341])

assert(model.checkPythonDump())
