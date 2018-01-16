# -*- coding: utf-8 -*-

## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

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
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Revolution_1_1/Generated_Face_2"))
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
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "Revolution_1_1/Generated_Face_2&Revolution_1_1/To_Face_1"), False)
SketchLine_8 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_8.result())
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "Revolution_1_1/Generated_Face_2&Revolution_1_1/From_Face_1"), False)
SketchLine_9 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_4.startPoint(), SketchLine_9.result())
Extrusion_1.setNestedSketch(Sketch_2)

Boolean_1 = model.addFill(Part_1_doc, [model.selection("SOLID", "Revolution_1_1")], [model.selection("SOLID", "Extrusion_1_1")])

Point_2 = model.addPoint(Part_1_doc, "x", "y", "-z")
Axis_4 = model.addAxis(Part_1_doc, model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Boolean_1_1_2/Modified_Face_1&Boolean_1_1_2/Modified_Face_3&Boolean_1_1_2/Modified_Face_4"))
Plane_4 = model.addPlane(Part_1_doc, model.selection("EDGE", "Axis_1"), model.selection("VERTEX", "Point_1"), True)
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Plane_1"), "haut_ext_tuyau", True)
Plane_5.result().setName("Plane_2 arrivee tuyau")
Plane_6 = model.addPlane(Part_1_doc, model.selection("FACE", "Boolean_1_1_2/Modified_Face_5"), model.selection("EDGE", "Boolean_1_1_2/Modified_Face_5&Boolean_1_1_2/Modified_Face_2"), 45)
Plane_7 = model.addPlane(Part_1_doc, model.selection("FACE", "Boolean_1_1_2/Modified_Face_6"), model.selection("EDGE", "Boolean_1_1_2/Modified_Face_6&Boolean_1_1_2/Modified_Face_2"), 45)
Plane_8 = model.addPlane(Part_1_doc, model.selection("FACE", "Boolean_1_1_2/Modified_Face_6"), model.selection("EDGE", "Boolean_1_1_2/Modified_Face_6&Boolean_1_1_2/Modified_Face_5"), "90+45")

Boolean_2 = model.addFill(Part_1_doc, [model.selection("SOLID", "Boolean_1_1_1")], [model.selection("FACE", "Plane_5")])
Boolean_3 = model.addFill(Part_1_doc, [model.selection("SOLID", "Boolean_2_1_2")], [model.selection("FACE", "Plane_3")])
Boolean_4 = model.addFill(Part_1_doc, [model.selection("SOLID", "Boolean_3_1_2")], [model.selection("FACE", "Plane_4")])
Boolean_4.result().setColor(102, 51, 51)
Boolean_4.result().subResult(0).setColor(153, 153, 76)
Boolean_4.result().subResult(1).setColor(0, 204, 204)
Boolean_4.result().subResult(2).setColor(51, 51, 102)
Boolean_4.result().subResult(3).setColor(102, 204, 102)
Boolean_4.result().subResult(4).setColor(204, 204, 0)

Union_1 = model.addUnion(Part_1_doc, [model.selection("SOLID", "Boolean_4_1_4"), model.selection("SOLID", "Boolean_4_1_2")])

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

Extrusion_2_objects = [model.selection("FACE", "Sketch_3/Face-SketchCircle_1_2f-SketchLine_11f-SketchLine_14r-SketchLine_16r"), model.selection("FACE", "Sketch_3/Face-SketchCircle_1_2f-SketchLine_10f-SketchLine_14f-SketchLine_17r"), model.selection("FACE", "Sketch_3/Face-SketchCircle_1_2f-SketchLine_13f-SketchLine_15r-SketchLine_17f"), model.selection("FACE", "Sketch_3/Face-SketchCircle_1_2f-SketchLine_12f-SketchLine_15f-SketchLine_16f")]
Extrusion_2 = model.addExtrusion(Part_1_doc, Extrusion_2_objects, model.selection(), 10, 0)
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_10r-SketchLine_11r-SketchLine_12r-SketchLine_13r")], model.selection(), "haut_int_tuyau", 0)

Sketch_4 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_2_1/To_Face_3"))
SketchProjection_6 = Sketch_4.addProjection(model.selection("VERTEX", "Extrusion_2_1/Generated_Face_2&Extrusion_2_1/Generated_Face_4&Extrusion_2_1/To_Face_4"), False)
SketchPoint_3 = SketchProjection_6.createdFeature()
SketchProjection_7 = Sketch_4.addProjection(model.selection("VERTEX", "Extrusion_2_1/Generated_Face_5&Extrusion_2_1/Generated_Face_7&Extrusion_2_1/To_Face_3"), False)
SketchPoint_4 = SketchProjection_7.createdFeature()
SketchArc_2 = Sketch_4.addArc(0, 0, -7.071067811865476, -7.071067811865472, 7.071067811865476, 7.071067811865472, True)
SketchConstraintCoincidence_26 = Sketch_4.setCoincident(SketchPoint_3.result(), SketchArc_2.startPoint())
SketchConstraintCoincidence_27 = Sketch_4.setCoincident(SketchPoint_4.result(), SketchArc_2.results()[1])
SketchLine_18 = Sketch_4.addLine(7.071067811865476, 7.071067811865472, -7.071067811865476, -7.071067811865472)
SketchConstraintCoincidence_28 = Sketch_4.setCoincident(SketchArc_2.endPoint(), SketchLine_18.startPoint())
SketchConstraintCoincidence_29 = Sketch_4.setCoincident(SketchAPI_Point(SketchPoint_3).coordinates(), SketchLine_18.endPoint())
SketchProjection_8 = Sketch_4.addProjection(model.selection("VERTEX", "Extrusion_2_1/Generated_Face_13&Extrusion_2_1/Generated_Face_12&Extrusion_2_1/To_Face_1"), False)
SketchPoint_5 = SketchProjection_8.createdFeature()
SketchConstraintCoincidence_30 = Sketch_4.setCoincident(SketchArc_2.endPoint(), SketchAPI_Point(SketchPoint_5).coordinates())
model.do()

Revolution_2 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_4/Face-SketchArc_2_2f-SketchLine_18r")], model.selection("EDGE", "Sketch_4/Edge-SketchLine_18"), 0, 180)

Boolean_5 = model.addCut(Part_1_doc, [model.selection("SOLID", "Revolution_2_1")], [model.selection("SOLID", "Extrusion_3_1")])
Boolean_5.result().setName("demi-sphère")

Plane_9 = model.addPlane(Part_1_doc, model.selection("VERTEX", "Extrusion_2_1/Generated_Face_10&Extrusion_2_1/Generated_Face_1&Extrusion_2_1/From_Face_4"), model.selection("VERTEX", "Extrusion_2_1/Generated_Face_10&Extrusion_2_1/Generated_Face_1&Extrusion_2_1/To_Face_4"), model.selection("VERTEX", "Extrusion_2_1/Generated_Face_2&Extrusion_2_1/Generated_Face_10&Extrusion_2_1/To_Face_4"))
Plane_10 = model.addPlane(Part_1_doc, model.selection("VERTEX", "Extrusion_2_1/Generated_Face_12&Extrusion_2_1/Generated_Face_11&Extrusion_2_1/To_Face_1"), model.selection("VERTEX", "Extrusion_2_1/Generated_Face_13&Extrusion_2_1/Generated_Face_12&Extrusion_2_1/To_Face_1"), model.selection("VERTEX", "Extrusion_2_1/Generated_Face_13&Extrusion_2_1/Generated_Face_12&Extrusion_2_1/From_Face_1"))
Plane_11 = model.addPlane(Part_1_doc, model.selection("VERTEX", "Extrusion_2_1/Generated_Face_5&Extrusion_2_1/Generated_Face_7&Extrusion_2_1/To_Face_3"), model.selection("VERTEX", "Extrusion_2_1/Generated_Face_3&Extrusion_2_1/Generated_Face_7&Extrusion_2_1/To_Face_3"), model.selection("VERTEX", "Extrusion_2_1/Generated_Face_5&Extrusion_2_1/Generated_Face_7&Extrusion_2_1/From_Face_3"))
Plane_12 = model.addPlane(Part_1_doc, model.selection("VERTEX", "Extrusion_2_1/Generated_Face_1&Extrusion_2_1/Generated_Face_4&Extrusion_2_1/To_Face_4"), model.selection("VERTEX", "Extrusion_2_1/Generated_Face_2&Extrusion_2_1/Generated_Face_4&Extrusion_2_1/To_Face_4"), model.selection("VERTEX", "Extrusion_2_1/Generated_Face_2&Extrusion_2_1/Generated_Face_4&Extrusion_2_1/From_Face_4"))

Recover_1 = model.addRecover(Part_1_doc, Boolean_5, [Extrusion_3.result()])

Plane_13 = model.addPlane(Part_1_doc, model.selection("FACE", "Recover_1_1/Shape4"), model.selection("EDGE", "Recover_1_1/Shape4&Recover_1_1/Shape6"), "90+45")
Plane_14 = model.addPlane(Part_1_doc, model.selection("FACE", "Recover_1_1/Shape1"), model.selection("EDGE", "Recover_1_1/Shape1&Recover_1_1/Shape6"), "90+45")
Plane_15 = model.addPlane(Part_1_doc, model.selection("FACE", "Recover_1_1/Shape2"), model.selection("EDGE", "Recover_1_1/Shape2&Recover_1_1/Shape6"), "90+45")
Plane_16 = model.addPlane(Part_1_doc, model.selection("FACE", "Recover_1_1/Shape6"), model.selection("EDGE", "Recover_1_1/Shape3&Recover_1_1/Shape6"), 45)

Boolean_6_objects_2 = [model.selection("FACE", "Plane_6"), model.selection("FACE", "Plane_7"), model.selection("FACE", "Plane_8"), model.selection("FACE", "Plane_9"), model.selection("FACE", "Plane_10"), model.selection("FACE", "Plane_11"), model.selection("FACE", "Plane_12"), model.selection("FACE", "Plane_13")]
Boolean_6 = model.addFill(Part_1_doc, [model.selection("SOLID", "demi-sphère")], Boolean_6_objects_2)

Union_2_objects = [model.selection("SOLID", "Boolean_6_1_16"), model.selection("SOLID", "Boolean_6_1_13"), model.selection("SOLID", "Boolean_6_1_14")]
Union_2 = model.addUnion(Part_1_doc, Union_2_objects)
Union_3_objects = [model.selection("SOLID", "Boolean_6_1_10/Boolean_6_1_10"), model.selection("SOLID", "Boolean_6_1_3/Boolean_6_1_3"), model.selection("SOLID", "Boolean_6_1_5/Boolean_6_1_5"), model.selection("SOLID", "Boolean_6_1_11/Boolean_6_1_11")]
Union_3 = model.addUnion(Part_1_doc, Union_3_objects)
Union_4_objects = [model.selection("SOLID", "Boolean_6_1_12/Boolean_6_1_12"), model.selection("SOLID", "Boolean_6_1_6/Boolean_6_1_6"), model.selection("SOLID", "Boolean_6_1_4/Boolean_6_1_4")]
Union_4 = model.addUnion(Part_1_doc, Union_4_objects)
Union_5_objects = [model.selection("SOLID", "Boolean_6_1_2/Boolean_6_1_2"), model.selection("SOLID", "Boolean_6_1_1/Boolean_6_1_1"), model.selection("SOLID", "Boolean_6_1_7/Boolean_6_1_7")]
Union_5 = model.addUnion(Part_1_doc, Union_5_objects)
Union_6_objects = [model.selection("SOLID", "Boolean_6_1_8/Boolean_6_1_8"), model.selection("SOLID", "Boolean_6_1_9/Boolean_6_1_9"), model.selection("SOLID", "Boolean_6_1_15/Boolean_6_1_15")]
Union_6 = model.addUnion(Part_1_doc, Union_6_objects)
Union_6.result().setColor(0, 0, 204)
Union_6.result().subResult(0).setColor(204, 102, 102)
Union_6.result().subResult(1).setColor(127, 254, 127)
Union_6.result().subResult(2).setColor(102, 51, 102)
Union_6.result().subResult(3).setColor(76, 76, 153)
Union_6.result().subResult(4).setColor(0, 0, 254)

Partition_1_objects = [model.selection("SOLID", "Recover_1_1"), model.selection("COMPSOLID", "Union_1_1"), model.selection("COMPSOLID", "Extrusion_2_1"), model.selection("COMPSOLID", "demi-sphère")]
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

assert(model.checkPythonDump())
