# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
Point_2 = model.addPoint(Part_1_doc, 10, 10, 0)
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_1")])
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-27.43492861072797, 41.55044273324706, 64.29883100829633, -35.76346801736826)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "Point_1"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.result(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchLine_2 = Sketch_1.addLine(64.5972559141328, -34.82170453629492, 63.39686648861513, 69.37209759863701)
SketchLine_3 = Sketch_1.addLine(63.39686648861513, 69.37209759863701, -27.43492861072797, 41.55044273324706)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_1.result())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
model.do()
Wire_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_3")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects)
Split_1 = model.addSplit(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], [model.selection("VERTEX", "Vertex_1_1")])
model.do()

model.checkResult(Split_1,model,1,[0],[0],[0],[4],[8])
model.testHaveNamingSubshapes(Split_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())
