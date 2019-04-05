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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchArc_1 = Sketch_1.addArc(28.24683132290313, 24.84281403912743, 0, 50, 56.77054208906745, 0, False)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_1.endPoint())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_2.result())
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchArc_1.startPoint(), SketchAPI_Line(SketchLine_1).startPoint(), 50)
SketchArc_2 = Sketch_1.addArc(38.14157263904823, 16.22495851193372, 56.77054208906745, 0, 34.15529753265601, 40.60522496371553, False)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchArc_2.startPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchArc_2.results()[1])
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchArc_1_2"), model.selection("EDGE", "Sketch_1/SketchArc_2_2")])
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], model.selection(), 70, 0)
Shell_1 = model.addShell(Part_1_doc, [model.selection("SHELL", "Extrusion_1_1")])
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/YOZ"), 30, False)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchLine_3 = Sketch_2.addLine(-64.49168769733997, 17.16789081864163, 146.7677170722727, 22.9901626941168)
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchLine_3")])
Split_1 = model.addSplit(Part_1_doc, [model.selection("SHELL", "Shell_1_1")], [model.selection("EDGE", "Edge_1_1")])
model.do()

model.checkResult(Split_1,model,1,[0],[0],[2],[8],[18])
model.testHaveNamingSubshapes(Split_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())
