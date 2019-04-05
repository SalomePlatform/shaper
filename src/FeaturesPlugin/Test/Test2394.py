# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

from GeomAPI import *
from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "R", "58")
model.addParameter(Part_1_doc, "cote_cube", "R/2")
model.addParameter(Part_1_doc, "theta", "54")
model.addParameter(Part_1_doc, "phi", "36")
model.addParameter(Part_1_doc, "pi", "3.141592653589793")
model.addParameter(Part_1_doc, "x", "R*sin(phi/180*pi)*cos(theta/180*pi)")
model.addParameter(Part_1_doc, "y", "R*sin(phi/180*pi)*sin(theta/180*pi)")
model.addParameter(Part_1_doc, "z", "R*cos(phi/180*pi)")
model.addParameter(Part_1_doc, "haut_ext_tuyau", "5")
model.addParameter(Part_1_doc, "haut_int_tuyau", "14")
Param_Diam = model.addParameter(Part_1_doc, "diam_tuyau", "10")
Revolution_1 = model.addRevolution(Part_1_doc, [], model.selection("EDGE", "PartSet/OZ"), 90, 0)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_1 = SketchProjection_2.createdFeature()
SketchArc_1 = Sketch_1.addArc(0, 0, 58, 0, 0, -58, True)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchArc_1.center())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_1.startPoint())
SketchLine_2 = Sketch_1.addLine(0, -58, 0, 0)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_2.endPoint())
SketchLine_3 = Sketch_1.addLine(0, 0, 58, 0)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_3.endPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), "R")
Revolution_1.setNestedSketch(Sketch_1)
Extrusion_1 = model.addExtrusion(Part_1_doc, [], model.selection(), 0, "cote_cube")
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3"))
SketchLine_4 = Sketch_2.addLine(29, 0, 0, 0)
SketchLine_5 = Sketch_2.addLine(0, 0, 0, 29)
SketchLine_6 = Sketch_2.addLine(0, 29, 29, 29)
SketchLine_7 = Sketch_2.addLine(29, 29, 29, 0)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_4.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_5.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_6.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_7.result())
SketchConstraintLength_2 = Sketch_2.setLength(SketchLine_6.result(), "cote_cube")
SketchConstraintEqual_1 = Sketch_2.setEqual(SketchLine_6.result(), SketchLine_7.result())
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3][Revolution_1_1/To_Face]"), False)
SketchLine_8 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_8.result())
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3][Revolution_1_1/From_Face]"), False)
SketchLine_9 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_4.startPoint(), SketchLine_9.result())
Extrusion_1.setNestedSketch(Sketch_2)
Split_1 = model.addSplit(Part_1_doc, [model.selection("SOLID", "Revolution_1_1")], [model.selection("SOLID", "Extrusion_1_1")])
Point_2 = model.addPoint(Part_1_doc, "x", "y", "-z")
Axis_4 = model.addAxis(Part_1_doc, model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "[Split_1_1_2/Modified_Face&Sketch_2/SketchLine_5&Revolution_1_1/To_Face][Split_1_1_2/Modified_Face&Sketch_2/SketchLine_4&Revolution_1_1/From_Face][Split_1_1_2/Modified_Face&Extrusion_1_1/To_Face&Sketch_1/SketchLine_3]"))
Plane_4 = model.addPlane(Part_1_doc, model.selection("EDGE", "Axis_1"), model.selection("VERTEX", "Point_1"), True)
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Plane_1"), "haut_ext_tuyau", True)
Plane_5.result().setName("Plane_2 arrivee tuyau")
Plane_6 = model.addPlane(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_2/SketchLine_6"), model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_2/SketchLine_6][Extrusion_1_1/From_Face]"), 45)
Plane_7 = model.addPlane(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_2/SketchLine_7"), model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_2/SketchLine_7][Extrusion_1_1/From_Face]"), 45)
Plane_8 = model.addPlane(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_2/SketchLine_7"), model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_2/SketchLine_6][Extrusion_1_1/Generated_Face&Sketch_2/SketchLine_7]"), "90+45")
Split_2 = model.addSplit(Part_1_doc, [model.selection("SOLID", "Split_1_1_1")], [model.selection("FACE", "Plane_5")])
Split_3 = model.addSplit(Part_1_doc, [model.selection("SOLID", "Split_2_1_2")], [model.selection("FACE", "Plane_3")])
Split_4 = model.addSplit(Part_1_doc, [model.selection("SOLID", "Split_3_1_2")], [model.selection("FACE", "Plane_4")])
Split_4.result().setColor(102, 51, 51)
Split_4.result().subResult(0).setColor(153, 153, 76)
Split_4.result().subResult(1).setColor(0, 204, 204)
Split_4.result().subResult(2).setColor(51, 51, 102)
Split_4.result().subResult(3).setColor(102, 204, 102)
Split_4.result().subResult(4).setColor(204, 204, 0)
Union_1 = model.addUnion(Part_1_doc, [model.selection("SOLID", "Split_4_1_4"), model.selection("SOLID", "Split_4_1_2")])
Point_3 = model.addPoint(Part_1_doc, model.selection("EDGE", "Axis_1"), model.selection("FACE", "Plane_2 arrivee tuyau"))
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_2 arrivee tuyau"))
SketchProjection_5 = Sketch_3.addProjection(model.selection("VERTEX", "Point_2"), False)
SketchPoint_2 = SketchProjection_5.createdFeature()
SketchCircle_1 = Sketch_3.addCircle(0, 0, 10)
SketchConstraintCoincidence_13 = Sketch_3.setCoincident(SketchPoint_2.result(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_3.setRadius(SketchCircle_1.results()[1], "diam_tuyau")
SketchLine_10 = Sketch_3.addLine(-5, 5, 5, 5)
SketchLine_11 = Sketch_3.addLine(5, 5, 5, -5)
SketchLine_12 = Sketch_3.addLine(5, -5, -5, -5)
SketchLine_13 = Sketch_3.addLine(-5, -5, -5, 5)
SketchConstraintCoincidence_14 = Sketch_3.setCoincident(SketchLine_13.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_15 = Sketch_3.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_16 = Sketch_3.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_17 = Sketch_3.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintHorizontal_3 = Sketch_3.setHorizontal(SketchLine_10.result())
SketchConstraintVertical_4 = Sketch_3.setVertical(SketchLine_11.result())
SketchConstraintHorizontal_4 = Sketch_3.setHorizontal(SketchLine_12.result())
SketchConstraintVertical_5 = Sketch_3.setVertical(SketchLine_13.result())
SketchConstraintEqual_2 = Sketch_3.setEqual(SketchLine_10.result(), SketchLine_13.result())
SketchLine_14 = Sketch_3.addLine(5, 5, 7.071067811865479, 7.071067811865475)
SketchConstraintCoincidence_18 = Sketch_3.setCoincident(SketchLine_10.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_19 = Sketch_3.setCoincident(SketchLine_14.endPoint(), SketchCircle_1.results()[1])
SketchLine_15 = Sketch_3.addLine(-5, -5, -7.071067811865472, -7.071067811865476)
SketchConstraintCoincidence_20 = Sketch_3.setCoincident(SketchLine_12.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_21 = Sketch_3.setCoincident(SketchLine_15.endPoint(), SketchCircle_1.results()[1])
SketchConstraintCollinear_1 = Sketch_3.setCollinear(SketchLine_14.result(), SketchLine_15.result())
SketchLine_16 = Sketch_3.addLine(7.071067811865478, -7.071067811865476, 5, -5)
SketchConstraintCoincidence_22 = Sketch_3.setCoincident(SketchLine_16.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_23 = Sketch_3.setCoincident(SketchLine_11.endPoint(), SketchLine_16.endPoint())
SketchLine_17 = Sketch_3.addLine(-5, 5, -7.071067811865472, 7.071067811865476)
SketchConstraintCoincidence_24 = Sketch_3.setCoincident(SketchLine_10.startPoint(), SketchLine_17.startPoint())
SketchConstraintCoincidence_25 = Sketch_3.setCoincident(SketchLine_17.endPoint(), SketchCircle_1.results()[1])
SketchConstraintCollinear_2 = Sketch_3.setCollinear(SketchLine_17.result(), SketchLine_16.result())
SketchConstraintEqual_3 = Sketch_3.setEqual(SketchLine_14.result(), SketchLine_15.result())
SketchConstraintEqual_4 = Sketch_3.setEqual(SketchLine_17.result(), SketchLine_16.result())
SketchConstraintLength_3 = Sketch_3.setLength(SketchLine_10.result(), "diam_tuyau")
model.do()
Extrusion_2_objects = [model.selection("FACE", "Sketch_3/Face-SketchCircle_1_2f-SketchLine_14r-SketchLine_11f-SketchLine_16r"), model.selection("FACE", "Sketch_3/Face-SketchCircle_1_2f-SketchCircle_1_2f-SketchLine_17r-SketchLine_10f-SketchLine_14f"), model.selection("FACE", "Sketch_3/Face-SketchCircle_1_2f-SketchLine_15r-SketchLine_13f-SketchLine_17f"), model.selection("FACE", "Sketch_3/Face-SketchCircle_1_2f-SketchLine_16f-SketchLine_12f-SketchLine_15f")]
Extrusion_2 = model.addExtrusion(Part_1_doc, Extrusion_2_objects, model.selection(), 10, 0)
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_13r-SketchLine_12r-SketchLine_11r-SketchLine_10r")], model.selection(), "haut_int_tuyau", 0)
Sketch_4 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_2_1_3/To_Face"))
SketchProjection_6 = Sketch_4.addProjection(model.selection("VERTEX", "[Extrusion_2_1_3/Generated_Face&Sketch_3/SketchCircle_1_2][Extrusion_2_1_3/Generated_Face&Sketch_3/SketchLine_15][Extrusion_2_1_3/To_Face]"), False)
SketchPoint_3 = SketchProjection_6.createdFeature()
SketchProjection_7 = Sketch_4.addProjection(model.selection("VERTEX", "[Extrusion_2_1_2/Generated_Face&Sketch_3/SketchCircle_1_2&weak_name_1][Extrusion_2_1_2/Generated_Face&Sketch_3/SketchLine_17][Extrusion_2_1_2/To_Face]"), False)
SketchPoint_4 = SketchProjection_7.createdFeature()
SketchArc_2 = Sketch_4.addArc(0, 0, -7.071067811865476, -7.071067811865472, 7.071067811865476, 7.071067811865472, True)
SketchConstraintCoincidence_26 = Sketch_4.setCoincident(SketchPoint_3.result(), SketchArc_2.startPoint())
SketchConstraintCoincidence_27 = Sketch_4.setCoincident(SketchPoint_4.result(), SketchArc_2.results()[1])
SketchLine_18 = Sketch_4.addLine(7.071067811865476, 7.071067811865472, -7.071067811865476, -7.071067811865472)
SketchConstraintCoincidence_28 = Sketch_4.setCoincident(SketchArc_2.endPoint(), SketchLine_18.startPoint())
SketchConstraintCoincidence_29 = Sketch_4.setCoincident(SketchAPI_Point(SketchPoint_3).coordinates(), SketchLine_18.endPoint())
SketchProjection_8 = Sketch_4.addProjection(model.selection("VERTEX", "[Extrusion_2_1_1/Generated_Face&Sketch_3/SketchCircle_1_2][Extrusion_2_1_1/Generated_Face&Sketch_3/SketchLine_14][Extrusion_2_1_1/To_Face]"), False)
SketchPoint_5 = SketchProjection_8.createdFeature()
SketchConstraintCoincidence_30 = Sketch_4.setCoincident(SketchArc_2.endPoint(), SketchAPI_Point(SketchPoint_5).coordinates())
model.do()
Revolution_2 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_4/Face-SketchArc_2_2f-SketchLine_18r")], model.selection("EDGE", "Sketch_4/SketchLine_18"), 0, 180)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Revolution_2_1")], [model.selection("SOLID", "Extrusion_3_1")])
Cut_1.result().setName("demi-sphere")
Plane_9 = model.addPlane(Part_1_doc, model.selection("VERTEX", "[Extrusion_2_1_1/Generated_Face&Sketch_3/SketchLine_11][Extrusion_2_1_1/Generated_Face&Sketch_3/SketchLine_16][Extrusion_2_1_1/From_Face]"), model.selection("VERTEX", "[Extrusion_2_1_1/Generated_Face&Sketch_3/SketchLine_11][Extrusion_2_1_1/Generated_Face&Sketch_3/SketchLine_16][Extrusion_2_1_1/To_Face]"), model.selection("VERTEX", "[Extrusion_2_1_1/Generated_Face&Sketch_3/SketchCircle_1_2][Extrusion_2_1_1/Generated_Face&Sketch_3/SketchLine_16][Extrusion_2_1_1/To_Face]"))
Plane_10 = model.addPlane(Part_1_doc, model.selection("VERTEX", "[Extrusion_2_1_1/Generated_Face&Sketch_3/SketchLine_14][Extrusion_2_1_1/Generated_Face&Sketch_3/SketchLine_11][Extrusion_2_1_1/To_Face]"), model.selection("VERTEX", "[Extrusion_2_1_1/Generated_Face&Sketch_3/SketchCircle_1_2][Extrusion_2_1_1/Generated_Face&Sketch_3/SketchLine_14][Extrusion_2_1_1/To_Face]"), model.selection("VERTEX", "[Extrusion_2_1_1/Generated_Face&Sketch_3/SketchCircle_1_2][Extrusion_2_1_1/Generated_Face&Sketch_3/SketchLine_14][Extrusion_2_1_1/From_Face]"))
Plane_11 = model.addPlane(Part_1_doc, model.selection("VERTEX", "[Extrusion_2_1_2/Generated_Face&Sketch_3/SketchCircle_1_2&weak_name_1][Extrusion_2_1_2/Generated_Face&Sketch_3/SketchLine_17][Extrusion_2_1_2/To_Face]"), model.selection("VERTEX", "[Extrusion_2_1_2/Generated_Face&Sketch_3/SketchLine_17][Extrusion_2_1_2/Generated_Face&Sketch_3/SketchLine_10][Extrusion_2_1_2/To_Face]"), model.selection("VERTEX", "[Extrusion_2_1_2/Generated_Face&Sketch_3/SketchCircle_1_2&weak_name_1][Extrusion_2_1_2/Generated_Face&Sketch_3/SketchLine_17][Extrusion_2_1_2/From_Face]"))
Plane_12 = model.addPlane(Part_1_doc, model.selection("VERTEX", "[Extrusion_2_1_3/Generated_Face&Sketch_3/SketchLine_15][Extrusion_2_1_3/Generated_Face&Sketch_3/SketchLine_13][Extrusion_2_1_3/To_Face]"), model.selection("VERTEX", "[Extrusion_2_1_3/Generated_Face&Sketch_3/SketchCircle_1_2][Extrusion_2_1_3/Generated_Face&Sketch_3/SketchLine_15][Extrusion_2_1_3/To_Face]"), model.selection("VERTEX", "[Extrusion_2_1_3/Generated_Face&Sketch_3/SketchCircle_1_2][Extrusion_2_1_3/Generated_Face&Sketch_3/SketchLine_15][Extrusion_2_1_3/From_Face]"))
Recover_1 = model.addRecover(Part_1_doc, Cut_1, [Extrusion_3.result()])
Plane_13 = model.addPlane(Part_1_doc, model.selection("FACE", "Recover_1_1/Modified_Face&Sketch_3/SketchLine_10"), model.selection("EDGE", "[Recover_1_1/Modified_Face&Sketch_3/SketchLine_10][Recover_1_1/Modified_Face&Extrusion_3_1/To_Face]"), "90+45")
Plane_14 = model.addPlane(Part_1_doc, model.selection("FACE", "Recover_1_1/Modified_Face&Sketch_3/SketchLine_13"), model.selection("EDGE", "[Recover_1_1/Modified_Face&Sketch_3/SketchLine_13][Recover_1_1/Modified_Face&Extrusion_3_1/To_Face]"), "90+45")
Plane_15 = model.addPlane(Part_1_doc, model.selection("FACE", "Recover_1_1/Modified_Face&Sketch_3/SketchLine_12"), model.selection("EDGE", "[Recover_1_1/Modified_Face&Sketch_3/SketchLine_12][Recover_1_1/Modified_Face&Extrusion_3_1/To_Face]"), "90+45")
Plane_16 = model.addPlane(Part_1_doc, model.selection("FACE", "Recover_1_1/Modified_Face&Extrusion_3_1/To_Face"), model.selection("EDGE", "[Recover_1_1/Modified_Face&Sketch_3/SketchLine_11][Recover_1_1/Modified_Face&Extrusion_3_1/To_Face]"), 45)
Split_5_objects_2 = [model.selection("FACE", "Plane_6"), model.selection("FACE", "Plane_7"), model.selection("FACE", "Plane_8"), model.selection("FACE", "Plane_9"), model.selection("FACE", "Plane_10"), model.selection("FACE", "Plane_11"), model.selection("FACE", "Plane_12"), model.selection("FACE", "Plane_13")]
Split_5 = model.addSplit(Part_1_doc, [model.selection("SOLID", "demi-sphere")], Split_5_objects_2)
Split_5.result().subResult(0).setName("Split_5_1_1")
Split_5.result().subResult(1).setName("Split_5_1_2")
Split_5.result().subResult(2).setName("Split_5_1_3")
Split_5.result().subResult(3).setName("Split_5_1_4")
Split_5.result().subResult(4).setName("Split_5_1_5")
Split_5.result().subResult(5).setName("Split_5_1_6")
Split_5.result().subResult(6).setName("Split_5_1_7")
Split_5.result().subResult(7).setName("Split_5_1_8")
Split_5.result().subResult(8).setName("Split_5_1_9")
Split_5.result().subResult(9).setName("Split_5_1_10")
Split_5.result().subResult(10).setName("Split_5_1_11")
Split_5.result().subResult(11).setName("Split_5_1_12")
Split_5.result().subResult(12).setName("Split_5_1_13")
Split_5.result().subResult(13).setName("Split_5_1_14")
Split_5.result().subResult(14).setName("Split_5_1_15")
Split_5.result().subResult(15).setName("Split_5_1_16")
Union_2_objects = [model.selection("SOLID", "Split_5_1_14"), model.selection("SOLID", "Split_5_1_15"), model.selection("SOLID", "Split_5_1_16")]
Union_2 = model.addUnion(Part_1_doc, Union_2_objects)
Union_2.result().subResult(0).setName("Union_2_1_1")
Union_2.result().subResult(1).setName("Union_2_1_2")
Union_2.result().subResult(2).setName("Union_2_1_3")
Union_2.result().subResult(3).setName("Union_2_1_4")
Union_2.result().subResult(4).setName("Union_2_1_5")
Union_2.result().subResult(5).setName("Union_2_1_6")
Union_2.result().subResult(6).setName("Union_2_1_7")
Union_2.result().subResult(7).setName("Union_2_1_8")
Union_2.result().subResult(8).setName("Union_2_1_9")
Union_2.result().subResult(9).setName("Union_2_1_10")
Union_2.result().subResult(10).setName("Union_2_1_11")
Union_2.result().subResult(11).setName("Union_2_1_12")
Union_2.result().subResult(12).setName("Union_2_1_13")
Union_2.result().subResult(13).setName("Union_2_1_14")
Union_3_objects = [model.selection("SOLID", "Union_2_1_8"), model.selection("SOLID", "Union_2_1_4"), model.selection("SOLID", "Union_2_1_9"), model.selection("SOLID", "Union_2_1_11")]
Union_3 = model.addUnion(Part_1_doc, Union_3_objects)
Union_3.result().subResult(0).setName("Union_3_1_1")
Union_3.result().subResult(1).setName("Union_3_1_2")
Union_3.result().subResult(2).setName("Union_3_1_3")
Union_3.result().subResult(3).setName("Union_3_1_4")
Union_3.result().subResult(4).setName("Union_3_1_5")
Union_3.result().subResult(5).setName("Union_3_1_6")
Union_3.result().subResult(6).setName("Union_3_1_7")
Union_3.result().subResult(7).setName("Union_3_1_8")
Union_3.result().subResult(8).setName("Union_3_1_9")
Union_3.result().subResult(9).setName("Union_3_1_10")
Union_3.result().subResult(10).setName("Union_3_1_11")
Union_4_objects = [model.selection("SOLID", "Union_3_1_5"), model.selection("SOLID", "Union_3_1_7"), model.selection("SOLID", "Union_3_1_9")]
Union_4 = model.addUnion(Part_1_doc, Union_4_objects)
Union_4.result().subResult(0).setName("Union_4_1_1")
Union_4.result().subResult(1).setName("Union_4_1_2")
Union_4.result().subResult(2).setName("Union_4_1_3")
Union_4.result().subResult(3).setName("Union_4_1_4")
Union_4.result().subResult(4).setName("Union_4_1_5")
Union_4.result().subResult(5).setName("Union_4_1_6")
Union_4.result().subResult(6).setName("Union_4_1_7")
Union_4.result().subResult(7).setName("Union_4_1_8")
Union_4.result().subResult(8).setName("Union_4_1_9")
Union_5_objects = [model.selection("SOLID", "Union_4_1_2"), model.selection("SOLID", "Union_4_1_1"), model.selection("SOLID", "Union_4_1_3")]
Union_5 = model.addUnion(Part_1_doc, Union_5_objects)
Union_5.result().subResult(0).setName("Union_5_1_1")
Union_5.result().subResult(1).setName("Union_5_1_2")
Union_5.result().subResult(2).setName("Union_5_1_3")
Union_5.result().subResult(3).setName("Union_5_1_4")
Union_5.result().subResult(4).setName("Union_5_1_5")
Union_5.result().subResult(5).setName("Union_5_1_6")
Union_5.result().subResult(6).setName("Union_5_1_7")
Union_6_objects = [model.selection("SOLID", "Union_5_1_1"), model.selection("SOLID", "Union_5_1_2"), model.selection("SOLID", "Union_5_1_3")]
Union_6 = model.addUnion(Part_1_doc, Union_6_objects)
Union_6.result().setColor(0, 0, 204)
Union_6.result().subResult(0).setName("Union_6_1_1")
Union_6.result().subResult(0).setColor(204, 102, 102)
Union_6.result().subResult(1).setName("Union_6_1_2")
Union_6.result().subResult(1).setColor(127, 254, 127)
Union_6.result().subResult(2).setName("Union_6_1_3")
Union_6.result().subResult(2).setColor(102, 51, 102)
Union_6.result().subResult(3).setName("Union_6_1_4")
Union_6.result().subResult(3).setColor(76, 76, 153)
Union_6.result().subResult(4).setName("Union_6_1_5")
Union_6.result().subResult(4).setColor(0, 0, 254)
Partition_1_objects = [model.selection("SOLID", "Recover_1_1"), model.selection("COMPSOLID", "Union_1_1"), model.selection("COMPSOLID", "Extrusion_2_1"), model.selection("COMPSOLID", "demi-sphere")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
Partition_1.result().setColor(102, 0, 0)
Partition_1.result().subResult(0).setColor(255, 255, 0)
Partition_1.result().subResult(1).setColor(0, 254, 0)
Partition_1.result().subResult(2).setColor(0, 255, 255)
Partition_1.result().subResult(3).setColor(255, 85, 0)
Partition_1.result().subResult(4).setColor(153, 0, 0)
Partition_1.result().subResult(5).setColor(254, 0, 254)
Partition_1.result().subResult(6).setColor(127, 127, 254)
Partition_1.result().subResult(7).setColor(102, 204, 102)
Partition_1.result().subResult(8).setColor(127, 254, 127)
Partition_1.result().subResult(9).setColor(153, 153, 76)
Partition_1.result().subResult(10).setColor(204, 102, 102)
Partition_1.result().subResult(11).setColor(127, 254, 254)
Partition_1.result().subResult(12).setColor(255, 255, 0)
Partition_1.result().subResult(13).setColor(102, 51, 102)
Partition_1.result().subResult(14).setColor(102, 204, 204)
Partition_1.result().subResult(15).setColor(254, 127, 254)
Partition_1.result().subResult(16).setColor(153, 153, 0)
Partition_1.result().subResult(17).setColor(153, 153, 0)
Partition_1.result().subResult(18).setColor(204, 0, 204)
Folder_1 = model.addFolder(Part_1_doc, Revolution_1, Union_1)
Folder_2 = model.addFolder(Part_1_doc, Point_3, Union_6)
model.do()

# check the result
model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [19])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [19])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.FACE, [199])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.EDGE, [814])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.VERTEX, [1628])
model.testResultsVolumes(Partition_1, [103867.289635450288187712430953979])

# change parameter and check validity of the result
Param_Diam.setValue(8)
model.do()

model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [19])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [19])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.FACE, [191])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.EDGE, [744])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.VERTEX, [1488])
model.testResultsVolumes(Partition_1, [103221.354557478349306620657444])

model.end()

# too long
#assert(model.checkPythonDump())
