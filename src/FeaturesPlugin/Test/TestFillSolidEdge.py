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
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Front"))
SketchLine_1 = Sketch_1.addLine(4, 10, 7, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "[Box_1_1/Top][Box_1_1/Front]"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_2.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "[Box_1_1/Bottom][Box_1_1/Front]"), False)
SketchLine_3 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.result())
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1")])
Fill_1 = model.addFill(Part_1_doc, [model.selection("SOLID", "Box_1_1")], [model.selection("EDGE", "Edge_1_1")])
model.do()

model.checkResult(Fill_1, model, 1, [0], [1], [7], [30], [60])
model.testHaveNamingSubshapes(Fill_1, model, Part_1_doc)

model.end()

assert(model.checkPythonDump())
