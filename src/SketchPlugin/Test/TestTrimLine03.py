# Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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
from salome.shaper import geom

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-38.2842712474619, 18.28427124746191, -10, -10)
SketchLine_2 = Sketch_1.addLine(-10, -10, -10, 20)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-10, 20, -25, 5)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_1.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 40)
SketchConstraintMirror_1 = Sketch_1.addMirror(SketchLine_2.result(), [SketchLine_1.result(), SketchLine_3.result()])
[SketchLine_4, SketchLine_5] = SketchConstraintMirror_1.mirrored()
model.do()

model.testNbSubFeatures(Sketch_1, "SketchLine", 5)
model.testNbSubFeatures(Sketch_1, "SketchConstraintCoincidence", 3)
model.testNbSubFeatures(Sketch_1, "SketchConstraintLength", 1)
model.testNbSubFeatures(Sketch_1, "SketchConstraintMirror", 1)

#perform trim
SketchTrim = Sketch_1.addTrim(SketchLine_1, geom.Pnt2d(-12, -8))
SketchTrim.execute()
model.do()

model.testNbSubFeatures(Sketch_1, "SketchLine", 5)
model.testNbSubFeatures(Sketch_1, "SketchConstraintCoincidence", 2)
model.testNbSubFeatures(Sketch_1, "SketchConstraintLength", 1)
model.testNbSubFeatures(Sketch_1, "SketchConstraintMirror", 0)

model.end()

assert(model.checkPythonDump())
