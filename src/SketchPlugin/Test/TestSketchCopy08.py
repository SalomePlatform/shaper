# Copyright (C) 2020  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-40, -72, 0, -72)
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_3 = SketchProjection_2.createdFeature()
SketchArc_1 = Sketch_1.addArc(0, 0, 0, -72, 72, 0, False)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchArc_1.center())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.result(), SketchArc_1.endPoint())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 40)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 72)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_1.endPoint())
SketchEllipticArc_1 = Sketch_1.addEllipticArc(25, 0, 25, 74.98833242578475, 72, 0, 25, 88.5, False)
[SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchLine_4, SketchLine_5] = SketchEllipticArc_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.result(), SketchEllipticArc_1.center())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchEllipticArc_1.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchEllipticArc_1.endPoint(), SketchAPI_Line(SketchLine_4).endPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_6 = Sketch_1.addLine(7.834161075251729, 88.5, 25, 88.5)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchEllipticArc_1.majorAxisPositive(), SketchLine_6.endPoint())
SketchArc_2 = Sketch_1.addArc(-40, -8.616197183098587, -40, -72, -94.21171510314713, 24.22579430808612, True)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_2.startPoint())
SketchArc_3 = Sketch_1.addArc(-60, 3.5, -94.21171510314713, 24.22579430808612, -60, 43.5, True)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchArc_3.startPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchArc_3.results()[1])
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_3.results()[1], 40)
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_4.result(), SketchLine_6.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_2).startPoint(), SketchAPI_Point(SketchPoint_1).coordinates(), 25, True)
SketchLine_7 = Sketch_1.addLine(-40, -72, -40, -8.616197183098587)
SketchLine_7.setAuxiliary(True)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_7.endPoint())
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_1.result(), SketchLine_7.result(), 90, type = "Direct")
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchArc_3.endPoint(), SketchAPI_Line(SketchLine_2).startPoint(), 60)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchArc_3.endPoint(), SketchLine_6.startPoint(), 45)
SketchLine_8 = Sketch_1.addLine(-60, 3.5, -60, 43.5)
SketchLine_8.setAuxiliary(True)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchArc_3.center(), SketchLine_8.startPoint())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_8.endPoint())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_7.result(), SketchLine_8.result())
SketchCircle_1 = Sketch_1.addCircle(-60, 66, 22.5)
SketchCircle_1.setAuxiliary(True)
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_8.result(), SketchCircle_1.center())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchLine_6.result(), SketchCircle_1.results()[1])
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_3.result(), 66, True)
SketchBSpline_1 = Sketch_1.addSpline(poles = [(-60, 43.5), (-40, 43.5), (-11.56553854601814, 88.5), (8.434461453981855, 88.5)], weights = [1, 3, 3, 1])
[SketchPoint_8, SketchPoint_9, SketchPoint_10, SketchPoint_11] = SketchBSpline_1.controlPoles(auxiliary = [0, 1, 2, 3])
[SketchLine_9, SketchLine_10, SketchLine_11] = SketchBSpline_1.controlPolygon(auxiliary = [0, 1, 2])
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_11).coordinates(), SketchLine_6.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_9.result(), SketchLine_11.result())
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_8).coordinates(), SketchArc_3.endPoint())
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_10).endPoint(), SketchLine_6.result())
SketchLine_12 = Sketch_1.addLine(-19.07878402833891, -6, -23.3, -6)
SketchLine_13 = Sketch_1.addLine(-23.3, -6, -23.3, 6)
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchLine_14 = Sketch_1.addLine(-23.3, 6, -19.07878402833891, 6)
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_14.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_13.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_12.result())
SketchArc_4 = Sketch_1.addArc(0, 0, -19.07878402833891, -6, -19.07878402833891, 6, False)
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchArc_4.center())
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchArc_4.endPoint())
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchLine_12.startPoint(), SketchArc_4.startPoint())
SketchPoint_12 = Sketch_1.addPoint(-23.3, 0)
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchPoint_12.coordinates(), SketchLine_3.result())
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchPoint_12.coordinates(), SketchLine_13.result())
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchArc_4.results()[1], 20)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_13.result(), 12)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchPoint_12.coordinates(), SketchAPI_Line(SketchLine_2).startPoint(), 23.3, True)
SketchBSplinePeriodic_1 = Sketch_1.addSpline(poles = [(-80, -23), (-80, 0), (-50, 0), (-50, -23)], periodic = True)
[SketchPoint_13, SketchPoint_14, SketchPoint_15, SketchPoint_16] = SketchBSplinePeriodic_1.controlPoles(auxiliary = [0, 1, 2, 3])
[SketchLine_15, SketchLine_16, SketchLine_17, SketchLine_18] = SketchBSplinePeriodic_1.controlPolygon(auxiliary = [0, 1, 2, 3])
SketchConstraintCollinear_1 = Sketch_1.setCollinear(SketchLine_16.result(), SketchLine_5.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_18.result(), SketchLine_16.result())
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_15.result(), SketchLine_17.result())
SketchConstraintPerpendicular_2 = Sketch_1.setPerpendicular(SketchLine_18.result(), SketchLine_15.result())
SketchEllipse_1 = Sketch_1.addEllipse(35, 45, 35, 67.91287847483518, 10)
[SketchPoint_17, SketchPoint_18, SketchPoint_19, SketchPoint_20, SketchPoint_21, SketchPoint_22, SketchPoint_23, SketchLine_19, SketchLine_20] = SketchEllipse_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
SketchConstraintParallel_2 = Sketch_1.setParallel(SketchLine_4.result(), SketchLine_19.result())
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_16.result(), 30)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_15.result(), 23)
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_19.result(), 50)
SketchConstraintLength_6 = Sketch_1.setLength(SketchLine_20.result(), 20)
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_17).startPoint(), SketchArc_4.center(), 50, True)
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_23).coordinates(), SketchLine_4.result())
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_17).coordinates(), SketchLine_3.result(), 45, True)
SketchCircle_2 = Sketch_1.addCircle(0, 66, 12)
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchLine_2.result(), SketchCircle_2.center())
SketchConstraintRadius_4 = Sketch_1.setRadius(SketchCircle_2.results()[1], 12)
SketchLine_21 = Sketch_1.addLine(-60, 66, 0, 66)
SketchLine_21.setAuxiliary(True)
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_21.startPoint())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_21.result())
SketchConstraintCoincidence_29 = Sketch_1.setCoincident(SketchLine_21.endPoint(), SketchCircle_2.center())
model.do()

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchArc_1_2f-SketchEllipticArc_1f-SketchLine_6r-SketchBSpline_1r-SketchArc_3_2f-SketchArc_2_2f-SketchLine_12f-SketchLine_13f-SketchLine_14f-SketchArc_4_2r-SketchBSplinePeriodic_1f-SketchEllipse_1r-SketchCircle_2_2r")], model.selection(), 10, 0)

Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Sketch_1_Copy = model.copySketch(Part_2_doc, Sketch_1)
Extrusion_2 = model.addExtrusion(Part_2_doc, [model.selection("FACE", Sketch_1_Copy.name())], model.selection(), 0, 10)

model.end()

error = model.compareSketches(Sketch_1, Sketch_1_Copy)
assert(error == ""), error

model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [16])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [84])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [168])
model.testResultsVolumes(Extrusion_1, [183140.3921289446])

model.testNbResults(Extrusion_2, 1)
model.testNbSubResults(Extrusion_2, [5])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.SOLID, [5])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.FACE, [31])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.EDGE, [126])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.VERTEX, [252])
model.testResultsVolumes(Extrusion_2, [213109.1306148178])

assert(partSet.size("Features") == 2)
assert(Part_1_doc.size("Features") == 2)
assert(Part_2_doc.size("Features") == 2)

assert(model.checkPythonDump())
