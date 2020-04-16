# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
param_p = model.addParameter(Part_1_doc, "p", "5")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 0, 0, 30.0000000000845)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchPoint_1.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(0, 30.0000000000845, 30.00000000002328, 30.00000000002326)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(30.00000000002328, 30.00000000002326, 29.99999999996195, 0)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchProjection_3 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_5 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.result())
SketchLine_6 = Sketch_1.addLine(29.99999999996195, 0, 0, 0)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_6.endPoint())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), 30)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_4.result(), 30)
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_4.result(), SketchLine_3.result())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_4.result(), SketchLine_1.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_6r-SketchLine_4r-SketchLine_3r-SketchLine_1r")], model.selection(), 30, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Sketch_1/Face-SketchLine_6r-SketchLine_4r-SketchLine_3r-SketchLine_1r"))
SketchLine_7 = Sketch_2.addLine(20, 7, 10, 7)
SketchLine_8 = Sketch_2.addLine(10, 7, 10, 0)
SketchLine_9 = Sketch_2.addLine(10, 0, 20, 0)
SketchLine_10 = Sketch_2.addLine(20, 0, 20, 7)
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_8.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_10.result())
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_6"), False)
SketchLine_11 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_11.result())
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchAPI_Line(SketchLine_11).endPoint(), SketchLine_8.endPoint(), 10, True)
SketchConstraintLength_3 = Sketch_2.setLength(SketchLine_9.result(), 10)
SketchConstraintLength_4 = Sketch_2.setLength(SketchLine_8.result(), 7)
model.do()
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_7r-SketchLine_8f-SketchLine_9f-SketchLine_10f_wire")], model.selection(), "p", 0, [model.selection("SOLID", "Extrusion_1_1")])
Fillet_1 = model.addFillet(Part_1_doc, [model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_6][Extrusion_1_1/To_Face]")], 3)
model.do()
# increase the extrusion-cut heigh to make face of a box divided into two
param_p.setValue(30);
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Fillet_1.feature()))
model.end()

assert(model.checkPythonDump())
