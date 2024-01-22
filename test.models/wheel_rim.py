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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-70.81663645026607, 190, 229.1833635497339, 190)
SketchLine_1.setAuxiliary(True)
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchLine_2 = Sketch_1.addLine(16.64097565470627, 190, 22.64097565470627, 190)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_1.result())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_1.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 6)
SketchArc_1 = Sketch_1.addArc(32.64097565470627, 190, 42.64097565470627, 190, 22.64097565470627, 190, True)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_1.result())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchArc_1.endPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_1.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 10)
SketchLine_3 = Sketch_1.addLine(42.64097565470627, 190, 147.6409756547063, 190)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_1.result())
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_3.result(), 105)
SketchLine_4 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OX"))
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_1.startPoint(), SketchLine_4.result(), 190)
SketchLine_5 = Sketch_1.addLine(167.6409756547062, 232.4170641625502, 167.6409756547062, 155.6475648742598)
SketchLine_5.setAuxiliary(True)
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_5.result())
SketchArc_2 = Sketch_1.addArc(147.6409756547063, 202.5, 147.6409756547063, 190, 160.1409756547063, 202.5, False)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_2.startPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_3.result(), SketchArc_2.results()[1])
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_2.results()[1], 12.5)
SketchLine_6 = Sketch_1.addLine(147.6409756547063, 202.5, 147.6409756547063, 190)
SketchLine_6.setAuxiliary(True)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_6.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_6.endPoint())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_6.endPoint())
SketchLine_7 = Sketch_1.addLine(160.1409756547063, 202.5, 147.6409756547063, 202.5)
SketchLine_7.setAuxiliary(True)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_7.endPoint())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchLine_7.endPoint())
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_6.result(), SketchLine_7.result(), 90)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_2.endPoint(), SketchLine_5.result(), 145)
SketchPoint_1 = Sketch_1.addPoint(167.6409756547062, 218.5857086760099)
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_5.result())
SketchArc_3 = Sketch_1.addArc(181.1409756547063, 202.5, 167.6409756547062, 218.5857086760099, 160.1409756547063, 202.5, False)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchArc_3.startPoint())
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchArc_3.endPoint())
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_7.startPoint(), SketchArc_3.endPoint())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchArc_3.results()[1])
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchArc_3.results()[1], SketchArc_2.results()[1])
model.do()

# check overconstraint message appears
assert Sketch_1.solverError().value() != "", "FAILED: Sketch should report over-constrained situation"
# remove duplicated Tangent constraint
Part_1_doc.removeFeature(SketchConstraintTangent_3.feature())
model.do()
assert Sketch_1.solverError().value() == "", "FAILED: Sketch should NOT report over-constrained situation"

# continue creating model
SketchLine_8 = Sketch_1.addLine(16.64097565470627, 190, 16.64097565470627, 184)
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_8.startPoint())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_8.result())
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_8.result(), 6)
SketchLine_9 = Sketch_1.addLine(16.64097565470627, 184, 169.1531000589648, 184)
SketchLine_9.setAuxiliary(True)
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_9.result())
SketchLine_10 = Sketch_1.addLine(16.64097565470627, 184, 18.2409756547065, 184)
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchLine_10.startPoint(), SketchLine_8.endPoint())
SketchLine_11 = Sketch_1.addLine(147.6409756547063, 190, 147.6409756547063, 184)
SketchLine_11.setAuxiliary(True)
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_9.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_11.result(), SketchLine_9.result())
SketchArc_4 = Sketch_1.addArc(147.6409756547063, 202.5, 147.6409756547063, 184, 166.1409756547063, 202.5, False)
SketchArc_4.setName("SketchArc_5")
SketchArc_4.result().setName("SketchArc_5")
SketchArc_4.results()[1].setName("SketchArc_5_2")
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchArc_2.center(), SketchArc_4.center())
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchArc_4.center())
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchArc_4.center())
SketchConstraintCoincidence_29 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchArc_4.startPoint())
SketchConstraintCoincidence_30 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchLine_7.result())
SketchLine_12 = Sketch_1.addLine(47.47337262898633, 184, 147.6409756547063, 184)
SketchArc_5 = Sketch_1.addArc(32.64097565470627, 190, 32.64097565470627, 174, 47.47337262898633, 184, False)
SketchArc_5.setName("SketchArc_10")
SketchArc_5.result().setName("SketchArc_10")
SketchArc_5.results()[1].setName("SketchArc_10_2")
SketchConstraintCoincidence_31 = Sketch_1.setCoincident(SketchArc_5.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_32 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_12.endPoint())
SketchConstraintCoincidence_33 = Sketch_1.setCoincident(SketchArc_4.startPoint(), SketchLine_12.endPoint())
SketchArc_6 = Sketch_1.addArc(181.1409756547063, 202.5, 166.1409756547063, 202.5, 171.498118511849, 213.9897921211111, True)
SketchConstraintCoincidence_34 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchArc_6.startPoint())
SketchConstraintCoincidence_35 = Sketch_1.setCoincident(SketchArc_6.center(), SketchArc_3.center())
SketchLine_13 = Sketch_1.addLine(181.1409756547063, 202.5, 167.6409756547062, 218.5857086760099)
SketchLine_13.setAuxiliary(True)
SketchConstraintCoincidence_36 = Sketch_1.setCoincident(SketchArc_6.center(), SketchLine_13.startPoint())
SketchConstraintCoincidence_37 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_13.endPoint())
SketchConstraintCoincidence_38 = Sketch_1.setCoincident(SketchArc_3.startPoint(), SketchLine_13.endPoint())
SketchConstraintCoincidence_39 = Sketch_1.setCoincident(SketchLine_13.result(), SketchArc_6.endPoint())
SketchLine_14 = Sketch_1.addLine(167.6409756547062, 218.5857086760099, 171.498118511849, 213.9897921211111)
SketchConstraintCoincidence_40 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_14.startPoint())
SketchConstraintCoincidence_41 = Sketch_1.setCoincident(SketchArc_3.startPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_42 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_43 = Sketch_1.setCoincident(SketchArc_6.endPoint(), SketchLine_14.endPoint())
SketchLine_15 = Sketch_1.addLine(32.64097565470627, 190, 32.64097565470627, 174)
SketchLine_15.setAuxiliary(True)
SketchConstraintCoincidence_44 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_15.startPoint())
SketchConstraintCoincidence_45 = Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchArc_5.startPoint())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_15.result())
SketchLine_16 = Sketch_1.addLine(32.64097565470627, 174, 12.64097565470628, 174)
SketchConstraintCoincidence_46 = Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchArc_7 = Sketch_1.addArc(12.64097565470628, 165, 12.64097565470628, 174, 3.640975654706275, 165, False)
SketchConstraintCoincidence_47 = Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchArc_7.startPoint())
SketchLine_17 = Sketch_1.addLine(3.640975654706275, 165, 3.640975654706275, 100)
SketchConstraintCoincidence_48 = Sketch_1.setCoincident(SketchArc_7.endPoint(), SketchLine_17.startPoint())
SketchLine_18 = Sketch_1.addLine(3.640975654706275, 100, -5.359024345293726, 100)
SketchConstraintCoincidence_49 = Sketch_1.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
SketchLine_19 = Sketch_1.addLine(-5.359024345293726, 100, -5.359024345293726, 165)
SketchConstraintCoincidence_50 = Sketch_1.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchLine_20 = Sketch_1.addLine(-5.359024345293726, 165, -5.359024345293726, 100)
SketchConstraintCoincidence_51 = Sketch_1.setCoincident(SketchLine_19.endPoint(), SketchLine_20.startPoint())
SketchConstraintCoincidence_52 = Sketch_1.setCoincident(SketchLine_18.endPoint(), SketchLine_20.endPoint())
SketchConstraintCoincidence_53 = Sketch_1.setCoincident(SketchLine_19.startPoint(), SketchLine_20.endPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_16.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_18.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_17.result())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_20.result())
SketchConstraintTangent_4 = Sketch_1.setTangent(SketchLine_16.result(), SketchArc_7.results()[1])
SketchConstraintTangent_5 = Sketch_1.setTangent(SketchLine_17.result(), SketchArc_7.results()[1])
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchArc_7.results()[1], 9)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_20.result(), 65)
SketchLine_21 = Sketch_1.addLine(12.64097565470628, 174, 12.64097565470628, 192.2067667108204)
SketchLine_21.setName("SketchLine_22")
SketchLine_21.result().setName("SketchLine_22")
SketchLine_21.setAuxiliary(True)
SketchConstraintCoincidence_54 = Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchLine_21.startPoint())
SketchConstraintCoincidence_55 = Sketch_1.setCoincident(SketchArc_7.startPoint(), SketchLine_21.startPoint())
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_21.result())
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_16.result(), 20)
SketchArc_8 = Sketch_1.addArc(12.64097565470628, 165, -5.359024345293726, 165, 12.64097565470625, 183, True)
SketchConstraintCoincidence_56 = Sketch_1.setCoincident(SketchArc_7.center(), SketchArc_8.center())
SketchConstraintCoincidence_57 = Sketch_1.setCoincident(SketchLine_19.endPoint(), SketchArc_8.startPoint())
SketchConstraintCoincidence_58 = Sketch_1.setCoincident(SketchLine_20.startPoint(), SketchArc_8.startPoint())
SketchConstraintCoincidence_59 = Sketch_1.setCoincident(SketchArc_8.endPoint(), SketchLine_21.result())
SketchLine_22 = Sketch_1.addLine(12.64097565470625, 183, 18.2409756547065, 183)
SketchLine_22.setName("SketchLine_23")
SketchLine_22.result().setName("SketchLine_23")
SketchConstraintCoincidence_60 = Sketch_1.setCoincident(SketchArc_8.endPoint(), SketchLine_22.startPoint())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_22.result())
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_18.startPoint(), SketchLine_19.result(), 9)
SketchLine_23 = Sketch_1.addLine(18.2409756547065, 183, 18.2409756547065, 184)
SketchLine_23.setName("SketchLine_24")
SketchLine_23.result().setName("SketchLine_24")
SketchConstraintCoincidence_61 = Sketch_1.setCoincident(SketchLine_22.endPoint(), SketchLine_23.startPoint())
SketchConstraintCoincidence_62 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_23.endPoint())
SketchConstraintVertical_7 = Sketch_1.setVertical(SketchLine_23.result())
SketchConstraintCoincidence_63 = Sketch_1.setCoincident(SketchArc_5.endPoint(), SketchLine_9.result())
SketchConstraintCoincidence_64 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_9.result())
SketchConstraintCoincidence_65 = Sketch_1.setCoincident(SketchArc_5.center(), SketchLine_15.startPoint())
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchLine_10.startPoint(), SketchLine_23.result(), 1.6, True)
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchLine_10.startPoint(), SketchLine_22.result(), 1, True)
SketchConstraintRadius_4 = Sketch_1.setRadius(SketchArc_6.results()[1], 15)
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_8f-SketchLine_10f-SketchLine_24r-SketchLine_23r-SketchArc_8_2f-SketchLine_19r-SketchLine_18r-SketchLine_17r-SketchArc_7_2r-SketchLine_16r-SketchArc_10_2f-SketchLine_12f-SketchArc_5_2f-SketchArc_6_2r-SketchLine_14r-SketchArc_3_2f-SketchArc_2_2r-SketchLine_3r-SketchArc_1_2r-SketchLine_2r")], model.selection("EDGE", "PartSet/OX"), 360, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchCircle_1 = Sketch_2.addCircle(0, 120, 7.5)
SketchLine_24 = Sketch_2.addLine(model.selection("EDGE", "PartSet/OZ"))
SketchLine_24.setName("SketchLine_25")
SketchLine_24.result().setName("SketchLine_25")
SketchConstraintCoincidence_66 = Sketch_2.setCoincident(SketchCircle_1.center(), SketchLine_24.result())
SketchLine_25 = Sketch_2.addLine(model.selection("EDGE", "PartSet/OY"))
SketchLine_25.setName("SketchLine_26")
SketchLine_25.result().setName("SketchLine_26")
SketchConstraintDistance_6 = Sketch_2.setDistance(SketchCircle_1.center(), SketchLine_25.result(), 120)
SketchConstraintRadius_5 = Sketch_2.setRadius(SketchCircle_1.results()[1], 7.5)
SketchMultiRotation_1 = Sketch_2.addRotation([SketchCircle_1.results()[1]], SketchLine_24.startPoint(), 24, 15)
[SketchCircle_2, SketchCircle_3, SketchCircle_4, SketchCircle_5, SketchCircle_6, SketchCircle_7, SketchCircle_8, SketchCircle_9, SketchCircle_10, SketchCircle_11, SketchCircle_12, SketchCircle_13, SketchCircle_14, SketchCircle_15] = SketchMultiRotation_1.rotated()
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), 10, 10)
Boolean_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Revolution_1_1")], [model.selection("SOLID", "Extrusion_1_6"), model.selection("SOLID", "Extrusion_1_8"), model.selection("SOLID", "Extrusion_1_9"), model.selection("SOLID", "Extrusion_1_3"), model.selection("SOLID", "Extrusion_1_2"), model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_1_4"), model.selection("SOLID", "Extrusion_1_14"), model.selection("SOLID", "Extrusion_1_5"), model.selection("SOLID", "Extrusion_1_15"), model.selection("SOLID", "Extrusion_1_11"), model.selection("SOLID", "Extrusion_1_10"), model.selection("SOLID", "Extrusion_1_12"), model.selection("SOLID", "Extrusion_1_13"), model.selection("SOLID", "Extrusion_1_7")])
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Boolean_1, 1)
model.testNbSubResults(Boolean_1, [0])
model.testNbSubShapes(Boolean_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Boolean_1, GeomAPI_Shape.FACE, [35])
model.testNbSubShapes(Boolean_1, GeomAPI_Shape.EDGE, [162])
model.testNbSubShapes(Boolean_1, GeomAPI_Shape.VERTEX, [324])
model.testResultsVolumes(Boolean_1, [2052731.260104598477482795715332031])

assert(model.checkPythonDump())
