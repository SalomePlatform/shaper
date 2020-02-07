# Copyright (C) 2020  CEA/DEN, EDF R&D
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
from GeomAPI import *
import math

ELL_CENTER_X = 10
ELL_CENTER_Y = 10
ELL_MAJOR_RAD = 30
ELL_MINOR_RAD = 15
DOF_1 = 5
DOF_2 = 9

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchEllipse_1 = Sketch_1.addEllipse(ELL_CENTER_X, ELL_CENTER_Y, ELL_CENTER_X + math.sqrt(ELL_MAJOR_RAD**2 + ELL_MINOR_RAD**2), ELL_CENTER_Y, ELL_MINOR_RAD)
[SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchLine_1, SketchLine_2] = SketchEllipse_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
model.do()
assert(model.dof(Sketch_1) == DOF_1)

# trim the ellipse
ANGLE = math.pi/4
Sketch_1.addTrim(SketchEllipse_1.feature(), GeomAPI_Pnt2d(ELL_CENTER_X + ELL_MAJOR_RAD * math.cos(ANGLE), ELL_CENTER_Y + ELL_MINOR_RAD * math.sin(ANGLE)))
model.do()
assert(model.dof(Sketch_1) == DOF_1)

Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchEllipse_2 = Sketch_2.addEllipse(ELL_CENTER_X, ELL_CENTER_Y, ELL_CENTER_X + math.sqrt(ELL_MAJOR_RAD**2 + ELL_MINOR_RAD**2), ELL_CENTER_Y, ELL_MINOR_RAD)
[SketchPoint_8, SketchPoint_9, SketchPoint_10, SketchPoint_11, SketchPoint_12, SketchPoint_13, SketchPoint_14, SketchLine_3, SketchLine_4] = SketchEllipse_2.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
SketchLine_5 = Sketch_2.addLine(15.23538168732762, 24.77570901315218, 37.44845404222143, 43.05543771157006)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_5.startPoint(), SketchEllipse_2.result())
SketchLine_6 = Sketch_2.addLine(37.44845404222143, 43.05543771157006, 37.66137837703927, 15.83721541173749)
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchEllipse_2.result())
model.do()
assert(model.dof(Sketch_2) == DOF_2)

# split the ellipse
Sketch_2.addSplit(SketchEllipse_2.feature(), GeomAPI_Pnt2d(ELL_CENTER_X + ELL_MAJOR_RAD * math.cos(ANGLE), ELL_CENTER_Y + ELL_MINOR_RAD * math.sin(ANGLE)))
DOF_2 += 3
model.do()
assert(model.dof(Sketch_2) == DOF_2)


model.end()

assert(model.checkPythonDump())
