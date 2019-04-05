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
model.addParameter(Part_1_doc, "L", "10")
model.addParameter(Part_1_doc, "Shift", "12")
Box_1 = model.addBox(Part_1_doc, "L", "L", "L")
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Bottom"), "L/2", True)
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchArc_1 = Sketch_1.addArc(0, 0, 15.49193338482974, -4, 0, -16, True)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchArc_1.center())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_1 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.result())
SketchLine_2 = Sketch_1.addLine(15.49193338482974, -4, 0, 12)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(0, 12, 0, -16)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_1.endPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_1.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], "L+Shift/2")
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchAPI_Line(SketchLine_1).startPoint(), SketchLine_2.startPoint(), "Shift/3")
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_1).startPoint(), SketchLine_3.startPoint(), "Shift", True)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchArc_1_2f-SketchLine_2f-SketchLine_3f_wire")])
Split_1 = model.addSplit(Part_1_doc, [model.selection("SOLID", "Box_1_1")], [model.selection("FACE", "Face_1_1")])
model.do()

model.checkResult(Split_1, model, 1, [2], [2], [12], [48], [96])
model.testHaveNamingSubshapes(Split_1, model, Part_1_doc)

model.end()

assert(model.checkPythonDump())
