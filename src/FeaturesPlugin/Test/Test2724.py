## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

# -*- coding: utf-8 -*-

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
model.addParameter(partSet, "b", "80")
model.addParameter(partSet, "a", "100")
model.addParameter(partSet, "c", "b-a")
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-60, -20, -60.00000000000001, 80)
SketchLine_2 = Sketch_1.addLine(-60.00000000000001, 80, 0, 0)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(0, 0, 0, -100)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(0, -100, -60, -20)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_3.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_4.result(), SketchLine_3.result())
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_2.result(), SketchLine_1.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_3.endPoint(), SketchLine_3.startPoint(), "a", True)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_4.endPoint(), SketchLine_2.endPoint(), 60)
SketchConstraintMirror_1_objects = [SketchLine_4.result(), SketchLine_1.result(), SketchLine_2.result()]
SketchConstraintMirror_1 = Sketch_1.addMirror(SketchLine_3.result(), SketchConstraintMirror_1_objects)
[SketchLine_5, SketchLine_6, SketchLine_7] = SketchConstraintMirror_1.mirrored()
SketchMultiTranslation_1_objects = [SketchLine_1.result(), SketchLine_4.result(), SketchLine_3.result(), SketchLine_5.result(), SketchLine_6.result()]
SketchMultiTranslation_1 = Sketch_1.addTranslation(SketchMultiTranslation_1_objects, SketchLine_2.endPoint(), SketchLine_4.startPoint(), 4)
[SketchLine_8, SketchLine_9, SketchLine_10, SketchLine_11, SketchLine_12, SketchLine_13, SketchLine_14, SketchLine_15, SketchLine_16, SketchLine_5, SketchLine_17, SketchLine_18, SketchLine_19, SketchLine_6, SketchLine_20, SketchLine_21, SketchLine_22] = SketchMultiTranslation_1.translated()
model.do()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Extrusion_1_objects = [model.selection("FACE", (-30, -10, 0)), model.selection("FACE", (30, -110, 0)), model.selection("FACE", (-30, -210, 0)), model.selection("FACE", (30, -310, 0))]
Extrusion_1 = model.addExtrusion(Part_1_doc, Extrusion_1_objects, model.selection(), "c", 0)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", (30, -310, -9.999999999999998))], model.selection("EDGE", (-30, -360, 0)), "-c")
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Placement_1 = model.addPlacement(Part_1_doc, [model.selection("SOLID", (-30.00000000000001, -10, -10))], model.selection("FACE", (5, 10, 5)), model.selection("FACE", (-30, 40, -10)), True, True)
Common_1 = model.addCommon(Part_1_doc, [model.selection("SOLID", (-30.00000000000001, -210, -10)), model.selection("SOLID", (18, -293.9999999999999, -9.999999999999998))])
model.do()
assert(model.checkPythonDump(model.ModelHighAPI.CHECK_WEAK))
model.end()
