# Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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
import math

def checkSketchLine(theLine, theX1, theY1, theX2, theY2, theTolerance = 1.e-7):
    assert(math.fabs(theLine.startPoint().x() - theX1) < theTolerance)
    assert(math.fabs(theLine.startPoint().y() - theY1) < theTolerance)
    assert(math.fabs(theLine.endPoint().x() - theX2) < theTolerance)
    assert(math.fabs(theLine.endPoint().y() - theY2) < theTolerance)

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
ParamD1 = model.addParameter(Part_1_doc, "D1", "80")
ParamD2 = model.addParameter(Part_1_doc, "D2", "30")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 80, 30, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_2.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_3 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_1.startPoint(), SketchAPI_Line(SketchLine_2).startPoint(), "D1", True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_1.endPoint(), SketchAPI_Line(SketchLine_2).startPoint(), "D2", True)
model.do()
model.end()
model.checkSketch(Sketch_1, 0)
checkSketchLine(SketchLine_1, 0, ParamD1.value(), ParamD2.value(), 0)

model.begin()
Sketch_1.setPlane(model.selection("FACE", "PartSet/YOZ"))
model.end()
model.checkSketch(Sketch_1, 0)
checkSketchLine(SketchLine_1, 0, ParamD1.value(), ParamD2.value(), 0)

model.begin()
Sketch_1.setPlane(model.selection("FACE", "PartSet/XOZ"))
model.end()
model.checkSketch(Sketch_1, 0)
checkSketchLine(SketchLine_1, 0, ParamD1.value(), ParamD2.value(), 0)

assert(model.checkPythonDump())
