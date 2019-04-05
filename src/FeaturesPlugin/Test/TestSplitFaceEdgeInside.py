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
SketchArc_1 = Sketch_1.addArc(-31.22351233671988, 15.68505079825835, -49.40058055152394, 17.44412191582002, -17.30100145268062, 27.5029960835475, True)
SketchLine_1 = Sketch_1.addLine(-17.30100145268062, 27.5029960835475, -6.596516690856308, -20.66908563134978)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.startPoint())
SketchLine_2 = Sketch_1.addLine(-6.596516690856308, -20.66908563134978, -34.74165457184325, -28.87808417997097)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-34.74165457184325, -28.87808417997097, -49.40058055152394, 17.44412191582002)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_3.endPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Sketch_1/Face-SketchArc_1_2r-SketchLine_3r-SketchLine_2r-SketchLine_1r"))
SketchLine_4 = Sketch_2.addLine(-17.59187701681758, -17.53169787967194, -23.97073271029336, 4.338664497959301)
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_2"), False)
SketchLine_5 = SketchProjection_1.createdFeature()
SketchConstraintPerpendicular_1 = Sketch_2.setPerpendicular(SketchLine_4.result(), SketchLine_5.result())
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchLine_4")])
Face_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchArc_1_2"), model.selection("EDGE", "Sketch_1/SketchLine_1")]
Face_1 = model.addFace(Part_1_doc, Face_1_objects)
Split_1 = model.addSplit(Part_1_doc, [model.selection("FACE", "Face_1_1")], [model.selection("EDGE", "Edge_1_1")])
model.do()

model.checkResult(Split_1,model,1,[0],[0],[1],[5],[10])
model.testHaveNamingSubshapes(Split_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())
