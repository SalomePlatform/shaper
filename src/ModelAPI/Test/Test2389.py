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
model.addParameter(Part_1_doc, "Width", "64")
model.addParameter(Part_1_doc, "Drill2_Loc", "6.5")
model.addParameter(Part_1_doc, "Drill2_Radius", "1.4")
model.addParameter(Part_1_doc, "Drill2_InnerRadius", "0.5")
model.addParameter(Part_1_doc, "Drill2_DepthMin", "6.5")
model.addParameter(Part_1_doc, "Drill2_DepthMax", "7.5")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(64, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 36)
SketchLine_3 = Sketch_1.addLine(0, 36, 64, 36)
SketchLine_4 = Sketch_1.addLine(64, 36, 64, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_3.startPoint(), SketchLine_3.endPoint(), "Width")
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 36)
SketchLine_5 = Sketch_1.addLine(22, 36, 22, 0)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_3.result())
SketchLine_6 = Sketch_1.addLine(32, 36, 32, 0)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchLine_3.result())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_1.result())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_1.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintDistanceHorizontal_2 = Sketch_1.setHorizontalDistance(SketchLine_2.endPoint(), SketchLine_5.startPoint(), "Width/32*11")
SketchConstraintDistanceHorizontal_3 = Sketch_1.setHorizontalDistance(SketchLine_6.startPoint(), SketchLine_3.endPoint(), "Width/2")
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_5_StartVertex"), True)
SketchPoint_2 = SketchProjection_2.createdFeature()
model.do()
model.do()
model.end()

assert(SketchProjection_2.external().namingName() != "")

assert(model.checkPythonDump())
