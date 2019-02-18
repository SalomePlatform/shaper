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
param = model.addParameter(Part_1_doc, "p", "60")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(30, 30, -20, 30)
SketchLine_2 = Sketch_1.addLine(-20, 30, -20, -20)
SketchLine_3 = Sketch_1.addLine(-20, -20, 30, -20)
SketchLine_4 = Sketch_1.addLine(30, -20, 30, 30)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_1.startPoint(), SketchLine_2.startPoint(), 50)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_4.endPoint(), SketchLine_3.endPoint(), 50)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintDistanceHorizontal_2 = Sketch_1.setHorizontalDistance(SketchLine_2.startPoint(), SketchAPI_Point(SketchPoint_1).coordinates(), 20)
SketchConstraintDistanceVertical_2 = Sketch_1.setVerticalDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_3.endPoint(), 20)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 10, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchLine_5 = Sketch_2.addLine(19.58333333333334, 17.27351642784226, -0.4166666666666646, 17.27351642784226)
SketchLine_6 = Sketch_2.addLine(-0.4166666666666646, 17.27351642784226, -0.4166666666794808, -42.72648357216971)
SketchLine_7 = Sketch_2.addLine(-0.4166666666794808, -42.72648357216971, 19.58333333332052, -42.72648357216971)
SketchLine_8 = Sketch_2.addLine(19.58333333332052, -42.72648357216971, 19.58333333333334, 17.27351642784226)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_8.result())
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/To_Face]"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_7.startPoint(), 30, True)
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1/To_Face]"), False)
SketchPoint_3 = SketchProjection_3.createdFeature()
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_3).coordinates(), SketchLine_7.endPoint(), 25, True)
SketchConstraintLength_1 = Sketch_2.setLength(SketchLine_7.result(), 20)
SketchConstraintLength_2 = Sketch_2.setLength(SketchLine_8.result(), "p")
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_5r-SketchLine_6f-SketchLine_7f-SketchLine_8f")], model.selection(), -5, -2)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], [model.selection("SOLID", "Extrusion_2_1")])
Box_1 = model.addBox(Part_1_doc, 50, 50, 30)
Placement_1 = model.addPlacement(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("FACE", "Box_1_1/Left"), model.selection("FACE", "Cut_1_1/Modified_Face&Extrusion_1_1/To_Face"), False, True)
model.do()
# change parameter to split extrusion top face to two
param.setValue(100)
model.end()

# check that placement is valid
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Placement_1.feature()))

assert(model.checkPythonDump(model.ModelHighAPI.CHECK_NAMING))
