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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-26.91983967935873, 43.92184368737476, -89.26052104208418, 43.92184368737476)
SketchLine_2 = Sketch_1.addLine(-89.26052104208418, 43.92184368737476, -89.26052104208418, 10.32264529058116)
SketchLine_3 = Sketch_1.addLine(-89.26052104208418, 10.32264529058116, -26.91983967935873, 10.32264529058116)
SketchLine_4 = Sketch_1.addLine(-26.91983967935873, 10.32264529058116, -26.91983967935873, 43.92184368737476)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(92.09418837675352, 44.32665330661324, 12.75150300601203, 44.32665330661324)
SketchLine_6 = Sketch_1.addLine(12.75150300601203, 44.32665330661324, 12.75150300601203, 10.32264529058116)
SketchLine_7 = Sketch_1.addLine(12.75150300601203, 10.32264529058116, 92.09418837675352, 10.32264529058116)
SketchLine_8 = Sketch_1.addLine(92.09418837675352, 10.32264529058116, 92.09418837675352, 44.32665330661324)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_9 = Sketch_2.addLine(-44.32665330661324, 21.65731462925853, -65.78156312625251, 21.65731462925853)
SketchLine_10 = Sketch_2.addLine(-65.78156312625251, 21.65731462925853, -65.78156312625251, -10.72745490981964)
SketchLine_11 = Sketch_2.addLine(-65.78156312625251, -10.72745490981964, -44.32665330661324, -10.72745490981964)
SketchLine_12 = Sketch_2.addLine(-44.32665330661324, -10.72745490981964, -44.32665330661324, 21.65731462925853)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_10.result())
SketchConstraintHorizontal_6 = Sketch_2.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_6 = Sketch_2.setVertical(SketchLine_12.result())
SketchLine_13 = Sketch_2.addLine(64.56713426853709, 24.08617234468937, 42.70741482965934, 24.08617234468937)
SketchLine_14 = Sketch_2.addLine(42.70741482965934, 24.08617234468937, 42.70741482965934, -10.32264529058116)
SketchLine_15 = Sketch_2.addLine(42.70741482965934, -10.32264529058116, 64.56713426853709, -10.32264529058116)
SketchLine_16 = Sketch_2.addLine(64.56713426853709, -10.32264529058116, 64.56713426853709, 24.08617234468937)
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_16.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintHorizontal_7 = Sketch_2.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_7 = Sketch_2.setVertical(SketchLine_14.result())
SketchConstraintHorizontal_8 = Sketch_2.setHorizontal(SketchLine_15.result())
SketchConstraintVertical_8 = Sketch_2.setVertical(SketchLine_16.result())
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f"), model.selection("FACE", "Sketch_1/Face-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_8f")])
Face_2 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_9r-SketchLine_10f-SketchLine_11f-SketchLine_12f"), model.selection("FACE", "Sketch_2/Face-SketchLine_13f-SketchLine_14f-SketchLine_15f-SketchLine_16f")])
Compound_1 = model.addCompound(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("FACE", "Face_1_2")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("FACE", "Face_2_1"), model.selection("FACE", "Face_2_2")])
Common_1 = model.addCommon(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1"), model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [2])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [16])

assert(model.checkPythonDump())
