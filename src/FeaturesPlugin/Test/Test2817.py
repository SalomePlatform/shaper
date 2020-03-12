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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 30)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 30)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f")], model.selection(), 100, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchLine_1 = Sketch_2.addLine(50, 10, -50, 10)
SketchLine_2 = Sketch_2.addLine(-50, 10, -50, 90)
SketchLine_3 = Sketch_2.addLine(-50, 90, 50, 90)
SketchLine_4 = Sketch_2.addLine(50, 90, 50, 10)
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_4.result())
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchCircle_1"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_1.result(), 10, True)
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_4.result(), 50, True)
SketchConstraintLength_1 = Sketch_2.setLength(SketchLine_1.result(), 100)
SketchConstraintLength_2 = Sketch_2.setLength(SketchLine_2.result(), 80)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")])
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("FACE", "Face_1_1")], model.selection("EDGE", "PartSet/OZ"), 12, keepSubResults = True)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("COMPOUND", "AngularCopy_1_1")], keepSubResults = True)
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Partition_1_1_3"))
SketchLine_5 = Sketch_3.addLine(50, 43.33333333333334, 35, 43.33333333333334)
SketchLine_6 = Sketch_3.addLine(35, 43.33333333333334, 35, 23.33333333333333)
SketchLine_7 = Sketch_3.addLine(35, 23.33333333333333, 50, 23.33333333333333)
SketchLine_8 = Sketch_3.addLine(50, 23.33333333333333, 50, 43.33333333333334)
SketchConstraintCoincidence_6 = Sketch_3.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_7 = Sketch_3.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8 = Sketch_3.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_9 = Sketch_3.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_3.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_3.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_3.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_3.setVertical(SketchLine_8.result())
SketchLine_9 = Sketch_3.addLine(50, 76.66666666666667, 35, 76.66666666666667)
SketchLine_10 = Sketch_3.addLine(35, 76.66666666666667, 35, 56.66666666666666)
SketchLine_11 = Sketch_3.addLine(35, 56.66666666666666, 50, 56.66666666666666)
SketchLine_12 = Sketch_3.addLine(50, 56.66666666666666, 50, 76.66666666666667)
SketchConstraintCoincidence_10 = Sketch_3.setCoincident(SketchLine_12.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_11 = Sketch_3.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_12 = Sketch_3.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_13 = Sketch_3.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintHorizontal_5 = Sketch_3.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_5 = Sketch_3.setVertical(SketchLine_10.result())
SketchConstraintHorizontal_6 = Sketch_3.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_6 = Sketch_3.setVertical(SketchLine_12.result())
SketchConstraintEqual_1 = Sketch_3.setEqual(SketchLine_8.result(), SketchLine_12.result())
SketchConstraintEqual_2 = Sketch_3.setEqual(SketchLine_11.result(), SketchLine_7.result())
SketchProjection_3 = Sketch_3.addProjection(model.selection("EDGE", "Partition_1_1_3/Modified_Edge&Sketch_2/SketchLine_2&Sketch_2/SketchLine_4"), False)
SketchLine_13 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_14 = Sketch_3.setCoincident(SketchLine_11.endPoint(), SketchLine_13.result())
SketchConstraintCoincidence_15 = Sketch_3.setCoincident(SketchLine_7.endPoint(), SketchLine_13.result())
SketchConstraintLength_3 = Sketch_3.setLength(SketchLine_7.result(), 15)
SketchConstraintLength_4 = Sketch_3.setLength(SketchLine_8.result(), 20)
SketchLine_14 = Sketch_3.addLine(50, 10, 50, 23.33333333333333)
SketchLine_14.setAuxiliary(True)
SketchConstraintCoincidence_16 = Sketch_3.setCoincident(SketchAPI_Line(SketchLine_13).endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_17 = Sketch_3.setCoincident(SketchLine_7.endPoint(), SketchLine_14.endPoint())
SketchLine_15 = Sketch_3.addLine(50, 43.33333333333334, 50, 56.66666666666666)
SketchLine_15.setAuxiliary(True)
SketchConstraintCoincidence_18 = Sketch_3.setCoincident(SketchLine_5.startPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_19 = Sketch_3.setCoincident(SketchLine_11.endPoint(), SketchLine_15.endPoint())
SketchLine_16 = Sketch_3.addLine(50, 76.66666666666667, 50, 90)
SketchLine_16.setAuxiliary(True)
SketchConstraintCoincidence_20 = Sketch_3.setCoincident(SketchLine_9.startPoint(), SketchLine_16.startPoint())
SketchConstraintCoincidence_21 = Sketch_3.setCoincident(SketchAPI_Line(SketchLine_13).startPoint(), SketchLine_16.endPoint())
SketchConstraintEqual_3 = Sketch_3.setEqual(SketchLine_14.result(), SketchLine_15.result())
SketchConstraintEqual_4 = Sketch_3.setEqual(SketchLine_15.result(), SketchLine_16.result())
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_3")], model.selection(), 5, 5)
AngularCopy_2 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Extrusion_2_1"), model.selection("SOLID", "Extrusion_2_2")], model.selection("EDGE", "PartSet/OZ"), 12, keepSubResults = True)
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPOUND", "Partition_1_1")], [model.selection("COMPOUND", "AngularCopy_2_1"), model.selection("COMPOUND", "AngularCopy_2_2")], keepSubResults = True)
model.end()

from GeomAPI import *

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [13])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [27])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [213])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [426])
model.testResultsVolumes(Cut_1, [282743.338823])

assert(model.checkPythonDump())
