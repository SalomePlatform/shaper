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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 0, 0, 3)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchPoint_1.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(0, 3, 4, 7)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(4, 7, 8, 3)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(8, 3, 8, 0)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchProjection_3 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_6 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.result())
SketchLine_7 = Sketch_1.addLine(8, 0, 0, 0)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_7.endPoint())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_4.result(), SketchLine_3.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_3.result(), SketchLine_4.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_5.result(), SketchLine_1.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 3)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_7.result(), 8)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_7r-SketchLine_5r-SketchLine_4r-SketchLine_3r-SketchLine_1r")], model.selection(), 5, 0)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5"), 5, True)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchLine_8 = Sketch_2.addLine(7.560741003605092, 9.43062705614625, -1.095841663628257, 9.43062705614625)
SketchLine_9 = Sketch_2.addLine(-1.095841663628257, 9.43062705614625, -1.095841663628257, -5.449377136847041)
SketchLine_10 = Sketch_2.addLine(-1.095841663628257, -5.449377136847041, 7.560741003605092, -5.449377136847041)
SketchLine_11 = Sketch_2.addLine(7.560741003605092, -5.449377136847041, 7.560741003605092, 9.43062705614625)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_8.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_9.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_10.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_11.result())
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_8r-SketchLine_9f-SketchLine_10f-SketchLine_11f")])
Split_1 = model.addSplit(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], [model.selection("FACE", "Face_1_1")])
model.do()

# now add an additional edge to the first sketch as it is described in the issue
SketchLine_8 = Sketch_1.addLine(0, 3, 8, 3)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_8.endPoint())
model.end()

# check that the plane is located as previously
model.testNbSubResults(Split_1, [2])
model.testResultsVolumes(Split_1, [120])
assert(model.checkPythonDump())
