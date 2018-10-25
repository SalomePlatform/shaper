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
model.addParameter(Part_1_doc, "R", "50")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-49.81325699578132, 35.39936274902501, 0, 8.200436015051352)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(0, 8.200436015051352, 49.94978427678259, 33.32234621934526)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
SketchArc_1 = Sketch_1.addArc(0, 31.08202242513997, 49.94978427678259, 33.32234621934526, -49.81325699578132, 35.39936274902501, False)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.endPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_2.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], "R")
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_4 = Sketch_2.addLine(31.79164821526215, 49.99496405877289, 8.200436015051352, 0)
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex"), False)
SketchPoint_1 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchPoint_1.result())
SketchLine_5 = Sketch_2.addLine(8.200436015051352, 0, 27.24680168984479, -49.85269382803262)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchArc_1"), False)
SketchPoint_2 = SketchProjection_3.createdFeature()
SketchArc_2 = Sketch_2.addArc(31.08202242513997, 1.147024884467139e-21, 27.24680168984479, -49.85269382803262, 31.79164821526215, 49.99496405877289, False)
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchPoint_2.result(), SketchArc_2.center())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchArc_2.endPoint(), SketchLine_4.startPoint())
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchArc_2.results()[1], "R")
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("WIRE", "[Sketch_1/SubEdge&Sketch_1/SketchLine_1][Sketch_1/SubEdge&Sketch_1/SketchLine_3][Sketch_1/SubEdge&Sketch_1/SketchArc_1_2]")])
Wire_2 = model.addWire(Part_1_doc, [model.selection("WIRE", "[Sketch_2/SubEdge&Sketch_2/SketchLine_4][Sketch_2/SubEdge&Sketch_2/SketchLine_5][Sketch_2/SubEdge&Sketch_2/SketchArc_2_2]")])
Fill_1 = model.addFill(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], [model.selection("WIRE", "Wire_2_1")])
model.do()

model.checkResult(Fill_1, model, 1, [0], [0], [0], [4], [8])
model.testHaveNamingSubshapes(Fill_1, model, Part_1_doc)

model.end()

assert(model.checkPythonDump())
