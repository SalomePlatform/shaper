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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchArc_1 = Sketch_1.addArc(9.761379814097372, -58.25586643592183, 0, 0, -48.40807467084933, -47.99209154562215, False)
SketchArc_2 = Sketch_1.addArc(-12.6706171814691, -54.29782746957817, -48.40807467084933, -47.99209154562215, 12.46009047944227, -80.47750863283412, False)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchArc_2.startPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchArc_2.results()[1])
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchAPI_Point(SketchPoint_1).coordinates())
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchArc_1_2"), model.selection("EDGE", "Sketch_1/SketchArc_2_2")])
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], model.selection(), 50, 0)
Shell_1 = model.addShell(Part_1_doc, [model.selection("SHELL", "Extrusion_1_1")])
Point_2 = model.addPoint(Part_1_doc, 0, 0, 10)
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_1")])
Split_1 = model.addSplit(Part_1_doc, [model.selection("SHELL", "Shell_1_1")], [model.selection("VERTEX", "Vertex_1_1")])
model.do()

model.checkResult(Split_1,model,1,[0],[0],[2],[9],[18])
model.testHaveNamingSubshapes(Split_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())
