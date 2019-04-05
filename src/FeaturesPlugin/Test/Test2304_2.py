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
SketchLine_1 = Sketch_1.addLine(345, 0, 0, 0)
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 200)
SketchLine_3 = Sketch_1.addLine(0, 200, 345, 200)
SketchLine_4 = Sketch_1.addLine(345, 200, 345, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 345)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), 200)
SketchLine_5 = Sketch_1.addLine(167, 200, 161.5499999999994, 190.5603230987492)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_3.result())
SketchLine_6 = Sketch_1.addLine(161.5499999999994, 190.5603230987492, 167, 181.1206461974984)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(167, 181.1206461974984, 177.9, 181.1206461974984)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(177.9, 181.1206461974984, 183.3499999999994, 190.5603230987492)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(183.3499999999994, 190.5603230987492, 177.9, 200)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_3.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintHorizontal_3.setName("SketchConstraintHorizontal_4")
SketchLine_10 = Sketch_1.addLine(167, 200, 167, 181.1206461974984)
SketchLine_10.setName("SketchLine_11")
SketchLine_10.result().setName("SketchLine_11")
SketchLine_10.setAuxiliary(True)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_12.setName("SketchConstraintCoincidence_14")
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_10.endPoint())
SketchConstraintCoincidence_13.setName("SketchConstraintCoincidence_15")
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_10.result())
SketchLine_11 = Sketch_1.addLine(177.9, 200, 177.9, 181.1206461974984)
SketchLine_11.setName("SketchLine_12")
SketchLine_11.result().setName("SketchLine_12")
SketchLine_11.setAuxiliary(True)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_14.setName("SketchConstraintCoincidence_16")
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_11.endPoint())
SketchConstraintCoincidence_15.setName("SketchConstraintCoincidence_17")
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_11.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_5.result(), SketchLine_6.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_9.result(), SketchLine_8.result())
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_9.result(), SketchLine_5.result())
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_7.result(), 10.9)
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchLine_7.result(), SketchLine_6.result())
SketchCircle_1 = Sketch_1.addCircle(172.4499999999994, 190.5603230987492, 10.89999999999998)
SketchCircle_1.setAuxiliary(True)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_16.setName("SketchConstraintCoincidence_18")
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_17.setName("SketchConstraintCoincidence_19")
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_18.setName("SketchConstraintCoincidence_20")
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_19.setName("SketchConstraintCoincidence_21")
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_5.startPoint(), SketchLine_2.result(), 167, False)
SketchLine_12 = Sketch_1.addLine(163.0544826719043, 187.9544826719043, 175.1, 200)
SketchLine_12.setName("SketchLine_13")
SketchLine_12.result().setName("SketchLine_13")
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchLine_12.startPoint(), SketchLine_6.result())
SketchConstraintCoincidence_20.setName("SketchConstraintCoincidence_22")
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_3.result())
SketchConstraintCoincidence_21.setName("SketchConstraintCoincidence_23")
SketchLine_13 = Sketch_1.addLine(163.0808822145049, 187.9087573228236, 175.1721248916813, 200)
SketchLine_13.setName("SketchLine_14")
SketchLine_13.result().setName("SketchLine_14")
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchLine_13.startPoint(), SketchLine_6.result())
SketchConstraintCoincidence_22.setName("SketchConstraintCoincidence_24")
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_3.result())
SketchConstraintCoincidence_23.setName("SketchConstraintCoincidence_25")
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_12.result(), SketchLine_13.result())
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_12.result(), SketchLine_13.startPoint(), 0.051, False)
SketchCircle_2 = Sketch_1.addCircle(163.0544826719043, 187.9544826719043, 0.004999999999999999)
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchLine_12.startPoint(), SketchCircle_2.center())
SketchConstraintCoincidence_24.setName("SketchConstraintCoincidence_26")
SketchCircle_3 = Sketch_1.addCircle(163.0808822145049, 187.9087573228236, 0.004999999999999999)
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchLine_13.startPoint(), SketchCircle_3.center())
SketchConstraintCoincidence_25.setName("SketchConstraintCoincidence_27")
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_2.results()[1], 0.005)
SketchConstraintEqual_5 = Sketch_1.setEqual(SketchCircle_2.results()[1], SketchCircle_3.results()[1])
SketchLine_14 = Sketch_1.addLine(163.0544826719043, 187.9544826719043, 163.0497273893228, 187.9529375869324)
SketchLine_14.setName("SketchLine_15")
SketchLine_14.result().setName("SketchLine_15")
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchLine_12.startPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_26.setName("SketchConstraintCoincidence_28")
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchCircle_2.results()[1])
SketchConstraintCoincidence_27.setName("SketchConstraintCoincidence_29")
SketchConstraintAngle_1 = Sketch_1.setAngleComplementary(SketchLine_6.result(), SketchLine_14.result(), 78.00000000013193)
SketchLine_15 = Sketch_1.addLine(163.0808822145049, 187.9087573228236, 163.0761269319234, 187.9072122378517)
SketchLine_15.setName("SketchLine_16")
SketchLine_15.result().setName("SketchLine_16")
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchLine_13.startPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_28.setName("SketchConstraintCoincidence_30")
SketchConstraintCoincidence_29 = Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchCircle_3.results()[1])
SketchConstraintCoincidence_29.setName("SketchConstraintCoincidence_31")
SketchConstraintAngle_2 = Sketch_1.setAngleComplementary(SketchLine_6.result(), SketchLine_15.result(), 78.00000000013193)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_5.startPoint(), SketchLine_12.endPoint(), 8.1, False)
SketchConstraintAngle_3 = Sketch_1.setAngle(SketchLine_3.result(), SketchLine_12.result(), 45)
model.do()
Edge_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2")]
Edge_1 = model.addEdge(Part_1_doc, Edge_1_objects)
Face_1_objects = [model.selection("EDGE", "Edge_1_1"), model.selection("EDGE", "Edge_1_2"), model.selection("EDGE", "Edge_1_3"), model.selection("EDGE", "Edge_1_4")]
Face_1 = model.addFace(Part_1_doc, Face_1_objects)
Edge_2_objects = [model.selection("EDGE", "Sketch_1/SketchLine_7"), model.selection("EDGE", "Sketch_1/SketchLine_6"), model.selection("EDGE", "Sketch_1/SketchLine_16"), model.selection("EDGE", "Sketch_1/SketchCircle_3_2"), model.selection("EDGE", "Sketch_1/SketchLine_15"), model.selection("EDGE", "Sketch_1/SketchCircle_2_2"), model.selection("EDGE", "Sketch_1/SketchLine_5"), model.selection("EDGE", "Sketch_1/SketchLine_13"), model.selection("EDGE", "Sketch_1/SketchLine_14"), model.selection("EDGE", "Sketch_1/SketchLine_8"), model.selection("EDGE", "Sketch_1/SketchLine_9")]
Edge_2 = model.addEdge(Part_1_doc, Edge_2_objects)
Partition_1_objects = [model.selection("FACE", "Face_1_1"), model.selection("EDGE", "Edge_2_1"), model.selection("EDGE", "Edge_2_2"), model.selection("EDGE", "Edge_2_3"), model.selection("EDGE", "Edge_2_4"), model.selection("EDGE", "Edge_2_5"), model.selection("EDGE", "Edge_2_6"), model.selection("EDGE", "Edge_2_7"), model.selection("EDGE", "Edge_2_8"), model.selection("EDGE", "Edge_2_9"), model.selection("EDGE", "Edge_2_10"), model.selection("EDGE", "Edge_2_11")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [12])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.FACE, [12])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.EDGE, [62])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.VERTEX, [124])

assert(model.checkPythonDump())
