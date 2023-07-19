# Copyright (C) 2014-2023  CEA, EDF
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
model.addParameter(Part_1_doc, "ob1", "4.37")
model.addParameter(Part_1_doc, "ob2", "4.38")
model.addParameter(Part_1_doc, "axe", "11.25")
Parameter_trou = model.addParameter(Part_1_doc, "trou", "7.5")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 0, 17.71094752491076, 14.8612495359528)
SketchLine_1.setAuxiliary(True)
SketchLine_2 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OX"))
SketchConstraintAngle_1 = Sketch_1.setAngleBackward(SketchLine_1.result(), SketchLine_2.result(), 40.00000000000006)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_2.startPoint())
SketchArc_1 = Sketch_1.addArc(0, 0, 17.71094752491076, 14.8612495359528, 23.12, -6.011798491948799e-033, True)
SketchArc_1.setAuxiliary(True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.center())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_1.endPoint())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_2.startPoint(), SketchArc_1.endPoint(), 23.12)
SketchArc_2 = Sketch_1.addArc(23.12, -6.011798491948799e-033, 18.74, 0, 27.5, 0, False)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchArc_2.center())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_2.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_2.endPoint())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_2.results()[1], "ob2")
SketchArc_3 = Sketch_1.addArc(0, 0, 18.74, 0, 14.35567286404965, 12.04583980552575, False)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_3.center())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchArc_3.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_3.endPoint())
SketchArc_4 = Sketch_1.addArc(0, 0, 27.5, 0, 21.0662221857694, 17.67665926638303, False)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_4.center())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchArc_4.startPoint())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchLine_1.result())
SketchArc_5 = Sketch_1.addArc(17.71094752491078, 14.86124953595279, 14.35567286404965, 12.04583980552575, 21.0662221857694, 17.67665926638303, True)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchArc_5.startPoint())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchArc_5.endPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_5.results()[1], SketchArc_3.results()[1])
SketchArc_6 = Sketch_1.addArc(-34.92, 0, -34.92, 4.370000000000002, -34.91999999999997, -4.370000000000019, False)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchArc_6.center(), SketchLine_2.result())
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_6.results()[1], "ob1")
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchArc_6.center(), SketchArc_2.center(), 58.04)
SketchLine_3 = Sketch_1.addLine(-24.92, 4.37, -34.92, 4.370000000000002)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchArc_6.startPoint(), SketchLine_3.endPoint())
SketchLine_4 = Sketch_1.addLine(-24.92, -4.37, -34.91999999999997, -4.370000000000019)
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchArc_6.endPoint(), SketchLine_4.endPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchLine_3.result(), SketchArc_6.results()[1])
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchLine_4.result(), SketchArc_6.results()[1])
SketchArc_7 = Sketch_1.addArc(-24.92, 1.467818159729891e-016, -24.92, 4.37, -24.92, -4.37, True)
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchArc_7.startPoint())
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchLine_4.startPoint(), SketchArc_7.endPoint())
SketchConstraintTangent_4 = Sketch_1.setTangent(SketchLine_3.result(), SketchArc_7.results()[1])
SketchConstraintTangent_5 = Sketch_1.setTangent(SketchArc_7.results()[1], SketchLine_4.result())
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchArc_6.center(), SketchArc_7.center(), 10)
SketchCircle_1 = Sketch_1.addCircle(9.498292007139793e-031, -4.263345141625117e-031, 5.625)
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_1.center())
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchCircle_1.results()[1], "axe/2")
SketchCircle_2 = Sketch_1.addCircle(-7.5, 18.75, 3.75)
SketchConstraintRadius_4 = Sketch_1.setRadius(SketchCircle_2.results()[1], "trou/2")
SketchLine_5 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OY"))
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchLine_5.result(), SketchCircle_2.center(), 7.5)
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchLine_2.result(), SketchCircle_2.center(), 18.75)
SketchArc_8 = Sketch_1.addArc(-34.92, -6.497717112746075e-027, -34.91999999999937, 7.500000000006299, -36.26235395189003, -7.378894623711957, False)
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchArc_6.center(), SketchArc_8.center())
SketchConstraintRadius_5 = Sketch_1.setRadius(SketchArc_8.results()[1], 7.5)
SketchArc_9 = Sketch_1.addArc(-7.5, 18.75, 0.0511741035599095, 21.7492323447386, -15.3796231578326, 20.73170731707517, False)
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchCircle_2.center(), SketchArc_9.center())
SketchConstraintRadius_6 = Sketch_1.setRadius(SketchArc_9.results()[1], "16.25/2")
SketchArc_10 = Sketch_1.addArc(-32.35111919008745, 25.0000000000063, -15.3796231578326, 20.73170731707517, -32.35111919008745, 7.500000000006299, True)
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchArc_9.endPoint(), SketchArc_10.startPoint())
SketchConstraintTangent_6 = Sketch_1.setTangent(SketchArc_9.results()[1], SketchArc_10.results()[1])
SketchConstraintRadius_7 = Sketch_1.setRadius(SketchArc_10.results()[1], 17.5)
SketchLine_6 = Sketch_1.addLine(-34.91999999999937, 7.500000000006299, -32.35111919008745, 7.500000000006299)
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchArc_8.startPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchArc_10.endPoint(), SketchLine_6.endPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintTangent_7 = Sketch_1.setTangent(SketchLine_6.result(), SketchArc_10.results()[1])
SketchArc_11 = Sketch_1.addArc(2.841203857564676e-031, -1.848261526152319e-031, -2.46098224513173, -13.52797347680525, 10.54941392733564, -8.818864200663077, False)
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_11.center())
SketchConstraintRadius_8 = Sketch_1.setRadius(SketchArc_11.results()[1], 13.75)
SketchLine_7 = Sketch_1.addLine(-36.26235395189003, -7.378894623711957, -2.460982245131729, -13.52797347680525)
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchArc_8.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_29 = Sketch_1.setCoincident(SketchArc_11.startPoint(), SketchLine_7.endPoint())
SketchConstraintTangent_8 = Sketch_1.setTangent(SketchLine_7.result(), SketchArc_8.results()[1])
SketchConstraintTangent_9 = Sketch_1.setTangent(SketchLine_7.result(), SketchArc_11.results()[1])
SketchArc_12 = Sketch_1.addArc(23.12, 7.384930033544359e-028, 18.58435121107267, -7.482672655108066, 31.87, -1.068593854246008e-030, False)
SketchConstraintCoincidence_30 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchArc_12.center())
SketchConstraintCoincidence_31 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_12.endPoint())
SketchArc_13 = Sketch_1.addArc(15.34460207612463, -12.82743883732818, 10.5494139273357, -8.818864200663123, 18.58435121107276, -7.482672655108122, True)
SketchConstraintCoincidence_32 = Sketch_1.setCoincident(SketchArc_11.endPoint(), SketchArc_13.startPoint())
SketchConstraintCoincidence_33 = Sketch_1.setCoincident(SketchArc_12.startPoint(), SketchArc_13.endPoint())
SketchConstraintTangent_10 = Sketch_1.setTangent(SketchArc_13.results()[1], SketchArc_12.results()[1])
SketchConstraintRadius_9 = Sketch_1.setRadius(SketchArc_12.results()[1], 8.75)
SketchConstraintTangent_11 = Sketch_1.setTangent(SketchArc_13.results()[1], SketchArc_11.results()[1])
SketchArc_14 = Sketch_1.addArc(7.217739748128488e-031, -7.397577026331382e-031, 31.87, 1.64056023214913e-033, 24.41383640220182, 20.48564112071002, False)
SketchConstraintCoincidence_34 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_14.center())
SketchConstraintCoincidence_35 = Sketch_1.setCoincident(SketchArc_12.endPoint(), SketchArc_14.startPoint())
SketchConstraintCoincidence_36 = Sketch_1.setCoincident(SketchArc_14.endPoint(), SketchLine_1.result())
SketchArc_15 = Sketch_1.addArc(17.71094752491077, 14.86124953595279, 24.41383640220183, 20.48564112071001, 10.79776038328427, 20.22504889320414, False)
SketchConstraintCoincidence_37 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchArc_15.center())
SketchConstraintCoincidence_38 = Sketch_1.setCoincident(SketchArc_14.endPoint(), SketchArc_15.startPoint())
SketchArc_16 = Sketch_1.addArc(5.859769567836763, 24.05633414838368, 10.79776038328427, 20.22504889320414, 0.05117410355990894, 21.7492323447386, True)
SketchConstraintCoincidence_39 = Sketch_1.setCoincident(SketchArc_15.endPoint(), SketchArc_16.startPoint())
SketchConstraintCoincidence_40 = Sketch_1.setCoincident(SketchArc_9.startPoint(), SketchArc_16.endPoint())
SketchConstraintTangent_12 = Sketch_1.setTangent(SketchArc_16.results()[1], SketchArc_15.results()[1])
SketchConstraintTangent_13 = Sketch_1.setTangent(SketchArc_16.results()[1], SketchArc_9.results()[1])
SketchConstraintRadius_10 = Sketch_1.setRadius(SketchArc_16.results()[1], 6.25)
SketchConstraintRadius_11 = Sketch_1.setRadius(SketchArc_13.results()[1], 6.25)
SketchConstraintTangent_14 = Sketch_1.setTangent(SketchLine_6.result(), SketchArc_8.results()[1])
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_8_2f-SketchLine_7f-SketchArc_11_2f-SketchArc_13_2r-SketchArc_12_2f-SketchArc_14_2f-SketchArc_15_2f-SketchArc_16_2r-SketchArc_9_2f-SketchArc_10_2r-SketchLine_6r-SketchCircle_1_2r-SketchArc_3_2f-SketchArc_5_2r-SketchArc_4_2r-SketchArc_2_2r-SketchLine_3r-SketchArc_7_2r-SketchLine_4f-SketchArc_6_2r-SketchCircle_2_2r")], model.selection(), 0, 2)
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchPoint_1 = Sketch_2.addPoint(model.selection("VERTEX", "Sketch_1/SketchArc_7"))
SketchArc_17 = Sketch_2.addArc(-24.92, -4.314010717669617e-016, -24.92, 7.500000000006299, -24.92, -7.500000000006299, True)
SketchConstraintCoincidence_41 = Sketch_2.setCoincident(SketchPoint_1.result(), SketchArc_17.center())
SketchPoint_2 = Sketch_2.addPoint(model.selection("VERTEX", "Sketch_1/SketchArc_8_2_StartVertex"))
SketchPoint_3 = Sketch_2.addPoint(model.selection("VERTEX", "Sketch_1/SketchArc_6"))
SketchArc_18 = Sketch_2.addArc(-34.92, 0, -34.91999999999937, 7.500000000006299, -34.92003434105975, -7.500000000006299, False)
SketchConstraintCoincidence_42 = Sketch_2.setCoincident(SketchPoint_3.result(), SketchArc_18.center())
SketchConstraintCoincidence_43 = Sketch_2.setCoincident(SketchPoint_2.result(), SketchArc_18.startPoint())
SketchLine_8 = Sketch_2.addLine(-34.91999999999937, 7.500000000006299, -24.92, 7.500000000006299)
SketchConstraintCoincidence_44 = Sketch_2.setCoincident(SketchPoint_2.coordinates(), SketchLine_8.startPoint())
SketchConstraintCoincidence_45 = Sketch_2.setCoincident(SketchArc_17.startPoint(), SketchLine_8.endPoint())
SketchLine_9 = Sketch_2.addLine(-34.92003434105975, -7.500000000006299, -24.92, -7.500000000006299)
SketchConstraintCoincidence_46 = Sketch_2.setCoincident(SketchArc_18.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_47 = Sketch_2.setCoincident(SketchArc_17.endPoint(), SketchLine_9.endPoint())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_8.result())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_9.result())
SketchConstraintTangent_15 = Sketch_2.setTangent(SketchLine_8.result(), SketchArc_17.results()[1])
SketchConstraintTangent_16 = Sketch_2.setTangent(SketchArc_17.results()[1], SketchLine_9.result())
SketchPoint_4 = Sketch_2.addPoint(model.selection("VERTEX", "Sketch_1/SketchCircle_2_2__cc"))
SketchCircle_3 = Sketch_2.addCircle(-7.5, 18.75, 8.125)
SketchConstraintCoincidence_48 = Sketch_2.setCoincident(SketchPoint_4.result(), SketchCircle_3.center())
SketchArc_19 = Sketch_2.addArc(model.selection("EDGE", "Sketch_1/SketchArc_9_2"))
SketchConstraintEqual_1 = Sketch_2.setEqual(SketchCircle_3.results()[1], SketchArc_19.results()[1])
SketchPoint_5 = Sketch_2.addPoint(model.selection("VERTEX", "Sketch_1/SketchCircle_1_2__cc"))
SketchCircle_4 = Sketch_2.addCircle(0, 0, 8.75)
SketchConstraintCoincidence_49 = Sketch_2.setCoincident(SketchPoint_5.result(), SketchCircle_4.center())
SketchConstraintRadius_12 = Sketch_2.setRadius(SketchCircle_4.results()[1], "17.5/2")
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchLine_8"), model.selection("EDGE", "Sketch_2/SketchArc_17_2"), model.selection("EDGE", "Sketch_2/SketchLine_9"), model.selection("EDGE", "Sketch_2/SketchArc_18_2"), model.selection("EDGE", "Sketch_1/SketchArc_6_2"), model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchArc_7_2"), model.selection("EDGE", "Sketch_1/SketchLine_4")])
Face_2 = model.addFace(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchCircle_3_2"), model.selection("EDGE", "Sketch_1/SketchCircle_2_2")])
Face_3 = model.addFace(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchCircle_4_2"), model.selection("EDGE", "Sketch_1/SketchCircle_1_2")])
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("FACE", "Face_2_1"), model.selection("FACE", "Face_3_1")], model.selection(), 2, 0)
Boolean_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_2_1"), model.selection("SOLID", "Extrusion_2_2"), model.selection("SOLID", "Extrusion_2_3")], [])
model.do()

# Test reexecution after parameter change
Parameter_trou.setValue(3.5)
model.do()
model.testResultsVolumes(Boolean_1, [4039.112573151546712324488908052])
Parameter_trou.setValue(7.5)
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Boolean_1, 1)
model.testNbSubShapes(Boolean_1, GeomAPI_Shape.SOLID, [1])
