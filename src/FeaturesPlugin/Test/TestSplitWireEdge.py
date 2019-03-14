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

# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_1 = Sketch_1.addLine(-67.57764876632801, 2.198838896952097, 88.39332365747461, 44.12336719883891)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_2 = Sketch_2.addLine(-40.60522496371553, -29.75761973875181, -38.84615384615385, 66.99129172714079)
SketchLine_3 = Sketch_2.addLine(-38.84615384615385, 66.99129172714079, 50.86647314949202, 73.4412191582003)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_2.addLine(50.86647314949202, 73.4412191582003, 55.26415094339622, -34.15529753265602)
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_2.addLine(55.26415094339622, -34.15529753265602, -40.60522496371553, -29.75761973875181)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_2.startPoint())
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1")])
Wire_1_objects = [model.selection("EDGE", "Sketch_2/SketchLine_5"), model.selection("EDGE", "Sketch_2/SketchLine_2"), model.selection("EDGE", "Sketch_2/SketchLine_3"), model.selection("EDGE", "Sketch_2/SketchLine_4")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects)
Split_1 = model.addSplit(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], [model.selection("EDGE", "Edge_1_1")])
model.do()

model.checkResult(Split_1,model,1,[0],[0],[0],[6],[12])
model.testHaveNamingSubshapes(Split_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())
