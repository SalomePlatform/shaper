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
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(22, 5, 10)
SketchCircle_2 = Sketch_1.addCircle(0, 0, 7.5)
SketchCircle_3 = Sketch_1.addCircle(29, -20, 7.5)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin", 1), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchCircle_2.center(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_1 = SketchProjection_2.createdFeature()
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_1.result(), 22, True)
SketchProjection_3 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_2 = SketchProjection_3.createdFeature()
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_2.result(), 5, True)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 10)
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchCircle_2.results()[1], 7.5)
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchCircle_3.results()[1], 7.5)
SketchLine_3 = Sketch_1.addLine(22, 5, 22, 41.96787958115183)
SketchLine_3.setAuxiliary(True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_3.startPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchCircle_3.center(), SketchLine_3.result(), 7, True)
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchCircle_3.center(), SketchLine_2.result(), 20, True)
SketchArc_1 = Sketch_1.addArc(22, 5, 19.39053135678072, 27.42761377547613, 44.57891020979248, 5, True)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchArc_1.center())
SketchLine_4 = Sketch_1.addLine(22, 5, 85.59353403141361, 5)
SketchLine_4.setAuxiliary(True)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(44.57891020979248, 5, 44.57891020979248, -20)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_5.startPoint())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_5.result(), 25)
model.end()

assert(model.checkPythonDump(model.ModelHighAPI.CHECK_WEAK))
