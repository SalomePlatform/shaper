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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(25, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 200)
SketchLine_3 = Sketch_1.addLine(0, 200, 25, 200)
SketchLine_3.setAuxiliary(True)
SketchLine_4 = Sketch_1.addLine(25, 200, 25, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 200)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_1.result(), 25)
SketchLine_5 = Sketch_1.addLine(125, 243.301270189222, -25, 243.3012701892219)
SketchLine_5.setAuxiliary(True)
SketchLine_6 = Sketch_1.addLine(-25, 243.3012701892219, 0, 200)
SketchLine_6.setAuxiliary(True)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_6.endPoint())
SketchLine_7 = Sketch_1.addLine(0, 200, 0, 350)
SketchLine_7.setAuxiliary(True)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_7.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_5.result(), SketchLine_7.result())
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_7.result(), SketchLine_6.result(), 30)
SketchLine_8 = Sketch_1.addLine(-25, 243.3012701892219, 29.37846722219903, 268.6583658936639)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(29.37846722219903, 268.6583658936639, 14.58682806127452, 300.3791384399466)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintPerpendicular_2 = Sketch_1.setPerpendicular(SketchLine_8.result(), SketchLine_9.result())
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_6.result(), 50)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_8.result(), 60)
SketchConstraintAngle_2 = Sketch_1.setAngle(SketchLine_5.result(), SketchLine_8.result(), 25)
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_9.result(), 35)
SketchLine_10 = Sketch_1.addLine(23.03919329608853, 282.2529826992136, 36.18065620811996, 288.3809474944538)
SketchLine_10.setAuxiliary(True)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_10.startPoint(), SketchLine_9.result())
SketchLine_11 = Sketch_1.addLine(36.18065620811996, 288.3809474944538, 48.85920406034094, 261.1917138833543)
SketchLine_11.setAuxiliary(True)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintPerpendicular_3 = Sketch_1.setPerpendicular(SketchLine_10.result(), SketchLine_11.result())
SketchConstraintPerpendicular_4 = Sketch_1.setPerpendicular(SketchLine_9.result(), SketchLine_10.result())
SketchConstraintLength_6 = Sketch_1.setLength(SketchLine_11.result(), 30)
SketchConstraintLength_7 = Sketch_1.setLength(SketchLine_10.result(), 14.5)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_9.endPoint(), SketchLine_10.startPoint(), 20, True)
SketchArc_1 = Sketch_1.addArc(4.697206021778518, 257.149304782984, 14.58682806127452, 300.3791384399466, 48.85920406034094, 261.1917138833543, True)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchArc_1.endPoint())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchArc_1.results()[1])
SketchArc_2 = Sketch_1.addArc(0.1512256873309691, 256.7331845326764, 48.85920406034094, 261.1917138833543, 34.14802119649147, 221.5683791690064, True)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchArc_2.startPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchArc_1.results()[1])
SketchArc_3 = Sketch_1.addArc(-71.98457930083825, 187.3071972226404, 0, 200, -25, 243.3012701892219, False)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchArc_3.startPoint())
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchArc_3.endPoint())
SketchLine_12 = Sketch_1.addLine(-25, 243.301270189222, 0, 222.3237794097899)
SketchLine_12.setAuxiliary(True)
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_7.result())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_3.results()[1], SketchLine_12.result())
SketchConstraintAngle_3 = Sketch_1.setAngle(SketchLine_6.result(), SketchLine_12.result(), 20)
SketchArc_4 = Sketch_1.addArc(55, 200, 34.14802119649147, 221.5683791690064, 25, 200, False)
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchArc_4.startPoint())
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchArc_4.results()[1])
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchLine_4.startPoint())
SketchConstraintTangent_4 = Sketch_1.setTangent(SketchLine_4.result(), SketchArc_4.results()[1])
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_4.results()[1], 30)
SketchLine_13 = Sketch_1.addLine(-7.179200577043265, 240.5774662449659, 15.47849409887298, 251.1429227884834)
SketchLine_14 = Sketch_1.addLine(15.47849409887298, 251.1429227884834, 26.04395064239047, 228.4852281125672)
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchLine_15 = Sketch_1.addLine(26.04395064239047, 228.4852281125672, 3.386255966474217, 217.9197715690497)
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchLine_16 = Sketch_1.addLine(3.386255966474217, 217.9197715690497, -7.179200577043265, 240.5774662449659)
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchLine_13.startPoint(), SketchLine_16.endPoint())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_13.result(), SketchLine_15.result())
SketchConstraintParallel_2 = Sketch_1.setParallel(SketchLine_16.result(), SketchLine_14.result())
SketchConstraintPerpendicular_5 = Sketch_1.setPerpendicular(SketchLine_13.result(), SketchLine_14.result())
SketchConstraintParallel_3 = Sketch_1.setParallel(SketchLine_13.result(), SketchLine_8.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_15.result(), SketchLine_14.result())
SketchConstraintLength_8 = Sketch_1.setLength(SketchLine_13.result(), 25)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_8.result(), SketchLine_16.endPoint(), 10, True)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchArc_3.endPoint(), SketchLine_16.result(), 15, True)
SketchConstraintLength_9 = Sketch_1.setLength(SketchLine_5.result(), 150)
SketchConstraintLength_10 = Sketch_1.setLength(SketchLine_7.result(), 150)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchArc_4_2r-SketchArc_2_2f-SketchArc_1_2f-SketchLine_9r-SketchLine_8r-SketchArc_3_2r-SketchLine_2r-SketchLine_1r-SketchLine_13f-SketchLine_14f-SketchLine_15f-SketchLine_16f")], model.selection(), 10, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_14"))
SketchLine_17 = Sketch_2.addLine(-221.0712923119171, 10, -196.0712923119172, 0)
SketchLine_17.setAuxiliary(True)
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_13][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_14][Extrusion_1_1/To_Face]"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_27 = Sketch_2.setCoincident(SketchLine_17.startPoint(), SketchPoint_2.result())
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_14][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_15][Extrusion_1_1/From_Face]"), False)
SketchPoint_3 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_28 = Sketch_2.setCoincident(SketchLine_17.endPoint(), SketchPoint_3.result())
SketchCircle_1 = Sketch_2.addCircle(-208.5712923119172, 5, 4)
SketchConstraintCoincidence_29 = Sketch_2.setCoincident(SketchLine_17.result(), SketchCircle_1.center())
SketchConstraintMiddle_1 = Sketch_2.setMiddlePoint(SketchCircle_1.center(), SketchLine_17.result())
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_1.results()[1], 4)
model.do()
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchCircle_1_2f_wire")], model.selection(), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_3_2"), 0, model.selection(), 5, [model.selection("SOLID", "Extrusion_1_1")])
model.do()

from GeomAPI import GeomAPI_Shape

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [18])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [90])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [180])
model.testResultsVolumes(ExtrusionCut_1, [78707.15553129233])

model.testHaveNamingSubshapes(ExtrusionCut_1, model, Part_1_doc)

model.end()

assert(model.checkPythonDump())
