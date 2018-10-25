## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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

# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "length_assemblage", "10")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 60, 10, 60)
SketchLine_2 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OY"))
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(10, 60, 10, 40)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(10, 40, 20, 40)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(20, 40, 20, 20)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(20, 20, 30, 20)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(30, 20, 30, 0)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OX"))
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.result())
SketchLine_9 = Sketch_1.addLine(30, 0, 81.26217508737108, 0)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_8.result())
SketchLine_10 = Sketch_1.addLine(0, 60, 0, 81.26217508737108)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_2.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_5.result())
SketchArc_1 = Sketch_1.addArc(4.151438510550382e-34, -3.089278765476956e-34, 81.26217508737108, 0, 0, 81.26217508737108, False)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchArc_1.center())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_1.endPoint())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchArc_1.endPoint())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_7.result())
SketchArc_2 = Sketch_1.addArc(4.151438510550382e-34, -3.089278765476956e-34, 76.26217508737108, 0, 0, 76.26217508737108, False)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchArc_2.center())
SketchConstraintCoincidence_16.setName("SketchConstraintCoincidence_23")
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_9.result(), SketchArc_2.startPoint())
SketchConstraintCoincidence_17.setName("SketchConstraintCoincidence_24")
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_10.result(), SketchArc_2.endPoint())
SketchConstraintCoincidence_18.setName("SketchConstraintCoincidence_25")
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_1.endPoint(), SketchArc_2.endPoint(), 5, False)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_7.result(), "2*length_assemblage")
SketchConstraintLength_1.setName("SketchConstraintLength_2")
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_6.result(), "length_assemblage")
SketchConstraintLength_2.setName("SketchConstraintLength_3")
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_5.result(), SketchLine_7.result())
SketchConstraintEqual_1.setName("SketchConstraintEqual_2")
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_6.result(), SketchLine_4.result())
SketchConstraintEqual_2.setName("SketchConstraintEqual_3")
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_5.result(), SketchLine_3.result())
SketchConstraintEqual_3.setName("SketchConstraintEqual_4")
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchLine_4.result(), SketchLine_1.result())
SketchConstraintEqual_4.setName("SketchConstraintEqual_5")
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "[Sketch_1/SubEdge&Sketch_1/SketchLine_1][Sketch_1/SubEdge&Sketch_1/SketchLine_3][Sketch_1/SubEdge&Sketch_1/SketchLine_4][Sketch_1/SubEdge&Sketch_1/SketchLine_5][Sketch_1/SubEdge&Sketch_1/SketchLine_6][Sketch_1/SubEdge&Sketch_1/SketchLine_7][Sketch_1/SubEdge&Sketch_1/SketchLine_9][Sketch_1/SketchArc_2_2][Sketch_1/SubEdge&Sketch_1/SketchLine_10]")])
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchLine_11 = Sketch_2.addLine(40, 0, 30, 0)
SketchLine_12 = Sketch_2.addLine(30, 0, 30, 10)
SketchLine_13 = Sketch_2.addLine(30, 10, 40, 10)
SketchLine_14 = Sketch_2.addLine(40, 10, 40, 0)
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_19.setName("SketchConstraintCoincidence_19")
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_20.setName("SketchConstraintCoincidence_20")
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_21.setName("SketchConstraintCoincidence_21")
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_22.setName("SketchConstraintCoincidence_26")
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_12.result())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_14.result())
SketchConstraintLength_3 = Sketch_2.setLength(SketchLine_13.result(), "length_assemblage")
SketchConstraintLength_3.setName("SketchConstraintLength_1")
SketchConstraintEqual_5 = Sketch_2.setEqual(SketchLine_13.result(), SketchLine_14.result())
SketchConstraintEqual_5.setName("SketchConstraintEqual_1")
model.do()
Sketch_2.setName("Sketch_3")
Sketch_2.result().setName("Sketch_3")
Face_2 = model.addFace(Part_1_doc, [model.selection("WIRE", "[Sketch_3/SubEdge&Sketch_3/SketchLine_14][Sketch_3/SubEdge&Sketch_3/SketchLine_13][Sketch_3/SubEdge&Sketch_3/SketchLine_12][Sketch_3/SubEdge&Sketch_3/SketchLine_11]")])
Face_2.setName("Face_5")
Face_2.result().setName("Face_5_1")
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("FACE", "Face_5_1")], model.selection("EDGE", "Sketch_1/SketchLine_9"), 10, 2)
LinearCopy_1.result().setName("LinearCopy")
Fill_1 = model.addFill(Part_1_doc, [model.selection("FACE", "Face_1_1")], [model.selection("COMPOUND", "LinearCopy")])
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Fill_1, 1)
model.testNbSubResults(Fill_1, [3])
model.testNbSubShapes(Fill_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Fill_1, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(Fill_1, GeomAPI_Shape.EDGE, [20])
model.testNbSubShapes(Fill_1, GeomAPI_Shape.VERTEX, [40])

assert(model.checkPythonDump())
