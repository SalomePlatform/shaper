# Copyright (C) 2019-2023  CEA, EDF
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

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, -20, 45, -15)
SketchLine_2 = Sketch_1.addLine(45, -15, 10, 15)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(10, 15, 25, -40)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchArc_1 = Sketch_2.addArc(-5, 10, -5, -10, 15, 10, False)
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1"), model.selection("COMPOUND", "Sketch_2")], False)
model.end()

model.testNbResults(Wire_1, 2)
model.testNbSubShapes(Wire_1, GeomAPI_Shape.EDGE, [3, 1])
model.testNbSubShapes(Wire_1, GeomAPI_Shape.VERTEX, [6, 2])

assert(model.checkPythonDump())
