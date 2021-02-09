# Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0.5, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 37.39521866225591, 0, 40)
SketchLine_3 = Sketch_1.addLine(0, 40, 0.5, 40)
SketchLine_4 = Sketch_1.addLine(1, 40, 1, 0.5)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 2.604781337744086)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_3.result(), 0.5)
SketchPoint_2 = Sketch_1.addPoint(0.5, 40)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_3.endPoint())
SketchLine_5 = Sketch_1.addLine(1, 40, 0.5, 40)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.startPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_5.endPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_4.startPoint(), SketchLine_5.result())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_5.endPoint())
SketchPoint_3 = Sketch_1.addPoint(0, 36.39521866225591)
SketchPoint_4 = Sketch_1.addPoint(0, 37.39521866225591)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchPoint_4.coordinates(), SketchPoint_3.coordinates(), 1)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchPoint_4.coordinates(), SketchLine_2.startPoint())
SketchLine_6 = Sketch_1.addLine(0, 36.39521866225591, 0, 37.39521866225591)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchPoint_3.coordinates(), SketchLine_6.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_6.endPoint())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_2.result())
SketchLine_7 = Sketch_1.addLine(0, 0, 0, 36.39521866225591)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchPoint_3.coordinates(), SketchLine_7.endPoint())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_6.result())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_7.result(), SketchLine_1.endPoint())
SketchLine_8 = Sketch_1.addLine(1, 0.5, 0.5, 0)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_8.startPoint(), SketchLine_4.endPoint())
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_8.startPoint(), SketchLine_4.endPoint())
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchLine_8.startPoint(), SketchLine_4.endPoint())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_7.result(), SketchLine_4.result())
SketchConstraintParallel_2 = Sketch_1.setParallel(SketchLine_2.result(), SketchLine_6.result())
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchLine_2.result(), SketchLine_6.endPoint())
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_6.result())
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchLine_3.result(), SketchLine_2.endPoint())
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_3.result())
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_1.result(), 0.5)
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_9 = SketchProjection_2.createdFeature()
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_8.startPoint(), SketchLine_9.result(), 0.5, True)
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchLine_4.result(), SketchLine_8.startPoint())
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_8.result())
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchLine_8.result(), SketchLine_8.startPoint())
SketchConstraintParallel_3 = Sketch_1.setParallel(SketchLine_3.result(), SketchLine_5.result())
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_5.result(), 0.5)
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchLine_5.result(), SketchLine_4.startPoint())
SketchConstraintCoincidence_29 = Sketch_1.setCoincident(SketchLine_5.result(), SketchLine_3.endPoint())
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_4.result(), 39.5)
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection("EDGE", "Sketch_1/SketchLine_4"), 360, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1"))
SketchLine_10 = Sketch_2.addLine(1, 1, 1, -1)
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1]"), False)
SketchCircle_1 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_30 = Sketch_2.setCoincident(SketchLine_10.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_31 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchCircle_1.results()[1])
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_10.result())
SketchConstraintCoincidence_32 = Sketch_2.setCoincident(SketchAPI_Circle(SketchCircle_1).center(), SketchLine_10.result())
SketchLine_11 = Sketch_2.addLine(0, 0, 2, 0)
SketchProjection_4 = Sketch_2.addProjection(model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1]"), False)
SketchPoint_5 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_33 = Sketch_2.setCoincident(SketchLine_11.startPoint(), SketchPoint_5.result())
SketchProjection_5 = Sketch_2.addProjection(model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1]"), False)
SketchCircle_2 = SketchProjection_5.createdFeature()
SketchConstraintCoincidence_34 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchCircle_2.results()[1])
SketchConstraintCoincidence_35 = Sketch_2.setCoincident(SketchAPI_Circle(SketchCircle_1).center(), SketchLine_11.result())
SketchLine_12 = Sketch_2.addLine(1.25, 0, 1.176776695296637, 0.1767766952966369)
SketchConstraintCoincidence_36 = Sketch_2.setCoincident(SketchLine_12.startPoint(), SketchLine_11.result())
SketchLine_13 = Sketch_2.addLine(1.176776695296637, 0.1767766952966369, 1, 0.25)
SketchConstraintCoincidence_37 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_38 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_10.result())
SketchProjection_6 = Sketch_2.addProjection(model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_8][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1]"), True)
SketchCircle_3 = SketchProjection_6.createdFeature()
SketchLine_14 = Sketch_2.addLine(1.176776695296637, 0.1767766952966369, 1.738436468990353, 0.6743230541046004)
SketchConstraintCoincidence_39 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_14.startPoint())
SketchProjection_7 = Sketch_2.addProjection(model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1]"), False)
SketchCircle_4 = SketchProjection_7.createdFeature()
SketchConstraintCoincidence_40 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchCircle_4.results()[1])
SketchCircle_5 = Sketch_2.addCircle(1, 0, 0.25)
SketchCircle_5.setAuxiliary(True)
SketchConstraintCoincidence_41 = Sketch_2.setCoincident(SketchAPI_Circle(SketchCircle_1).center(), SketchCircle_5.center())
SketchConstraintCoincidence_42 = Sketch_2.setCoincident(SketchLine_12.startPoint(), SketchCircle_5.results()[1])
SketchConstraintCoincidence_43 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchCircle_5.results()[1])
SketchConstraintRadius_1 = Sketch_2.setRadius(SketchCircle_5.results()[1], 0.25)
SketchConstraintEqual_1 = Sketch_2.setEqual(SketchLine_13.result(), SketchLine_12.result())
SketchConstraintCoincidence_44 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchCircle_5.results()[1])
SketchConstraintMirror_1_objects = [SketchLine_13.result(), SketchLine_12.result(), SketchLine_14.result()]
SketchConstraintMirror_1 = Sketch_2.addMirror(SketchLine_10.result(), SketchConstraintMirror_1_objects)
[SketchLine_15, SketchLine_16, SketchLine_17] = SketchConstraintMirror_1.mirrored()
SketchConstraintMirror_2_objects = [SketchLine_14.result(), SketchLine_12.result(), SketchLine_13.result(), SketchLine_15.result(), SketchLine_16.result(), SketchLine_17.result()]
SketchConstraintMirror_2 = Sketch_2.addMirror(SketchLine_11.result(), SketchConstraintMirror_2_objects)
[SketchLine_18, SketchLine_19, SketchLine_20, SketchLine_21, SketchLine_22, SketchLine_23] = SketchConstraintMirror_2.mirrored()
model.do()
Edge_1_objects = [model.selection("EDGE", "Sketch_2/SketchLine_23"), model.selection("EDGE", "Sketch_2/SketchLine_21"), model.selection("EDGE", "Sketch_2/SketchLine_22"), model.selection("EDGE", "Sketch_2/SketchLine_10"), model.selection("EDGE", "Sketch_2/SketchLine_15"), model.selection("EDGE", "Sketch_2/SketchLine_16"), model.selection("EDGE", "Sketch_2/SketchLine_17"), model.selection("EDGE", "Sketch_2/SketchLine_11"), model.selection("EDGE", "Sketch_2/SketchLine_12"), model.selection("EDGE", "Sketch_2/SketchLine_20"), model.selection("EDGE", "Sketch_2/SketchLine_19"), model.selection("EDGE", "Sketch_2/SketchLine_18"), model.selection("EDGE", "Sketch_2/SketchLine_13"), model.selection("EDGE", "Sketch_2/SketchLine_14"), model.selection("EDGE", "Sketch_2/SketchProjection_6")]
Edge_1 = model.addEdge(Part_1_doc, Edge_1_objects)
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Edge_1")], model.selection("EDGE", "PartSet/OY"), model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3"), 0, model.selection(), 0)
model.end()

from GeomAPI import *

model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [15])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [15])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [102])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [204])
model.testResultsAreas(Extrusion_1, [466.947878])

assert(model.checkPythonDump())
