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
param = model.addParameter(Part_1_doc, "p", "10")
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))
SketchLine_1 = Sketch_1.addLine(6.82842712474619, 7.171572875253809, 2.82842712474619, 7.171572875253809)
SketchLine_2 = Sketch_1.addLine(2.82842712474619, 7.171572875253809, 2.82842712474619, 3.171572875253809)
SketchLine_3 = Sketch_1.addLine(2.82842712474619, 3.171572875253809, 6.82842712474619, 3.171572875253809)
SketchLine_4 = Sketch_1.addLine(6.82842712474619, 3.171572875253809, 6.82842712474619, 7.171572875253809)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchCircle_1 = Sketch_1.addCircle(4.440892098500626e-16, 0, 1)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 4)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_4.result(), 4)
SketchLine_5 = Sketch_1.addLine(0, 10, 10, 0)
SketchLine_5.setAuxiliary(True)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Right][Box_1_1/Top]"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchPoint_1.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Top]"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchPoint_2.result())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_5.result(), SketchLine_2.startPoint())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_2.startPoint(), SketchLine_5.startPoint(), 4, True)
SketchProjection_3 = Sketch_1.addProjection(model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), False)
SketchLine_6 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_6.result(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 1)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_6).endPoint(), SketchCircle_1.center(), "p", True)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f")], model.selection(), 1, 1)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Box_1_1")], [model.selection("SOLID", "Extrusion_1_1")])
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f_wire")], model.selection("EDGE", "[Cut_1_1/Modified_Face&Box_1_1/Left][Cut_1_1/Modified_Face&Box_1_1/Top]"), 45, 0)
model.do()
# relocate circle to split the edge - axis of revolution
param.setValue(5)
model.end()

# check that revolution is valid
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Revolution_1.feature()))

assert(model.checkPythonDump(model.ModelHighAPI.CHECK_NAMING))
