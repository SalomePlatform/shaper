# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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
from ModelAPI import *
import math

def assertLineLength(theLine, theLength):
    dx = theLine.startPoint().x() - theLine.endPoint().x()
    dy = theLine.startPoint().y() - theLine.endPoint().y()
    assert(math.fabs(dx**2 + dy**2 - theLength**2)  < 1.e-8), "Line length {} is not equal to expected {}".format(math.sqrt(dx**2 + dy**2), theLength)


model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
ParamLen = model.addParameter(Part_1_doc, "Len", "100")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, -30, 100, -30)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), "Len")
model.do()

aLength = ParamLen.value()
assertLineLength(SketchLine_1, aLength)

# delete parameter
model.removeParameter(Part_1_doc, ParamLen)
model.do()

# move line and check the constraint is still here
SketchLine_1.startPoint().setValue(SketchLine_1.startPoint().x() + 10., SketchLine_1.startPoint().y())
model.do()
assertLineLength(SketchLine_1, aLength)

model.end()
