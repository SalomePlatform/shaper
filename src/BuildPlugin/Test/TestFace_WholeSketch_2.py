# Copyright (C) 2019  CEA/DEN, EDF R&D
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
SketchCircle_1 = Sketch_2.addCircle(-5, 10, 20)
SketchLine_4 = Sketch_2.addLine(-25, 10, 15, 10)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_4.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchCircle_1.results()[1])
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1"), model.selection("COMPOUND", "Sketch_2")])
model.end()

model.testNbResults(Face_1, 3)
model.testNbSubShapes(Face_1, GeomAPI_Shape.FACE, [1, 1, 1])
model.testNbSubShapes(Face_1, GeomAPI_Shape.EDGE, [3, 2, 2])
model.testNbSubShapes(Face_1, GeomAPI_Shape.VERTEX, [6, 4, 4])
model.testResultsVolumes(Face_1, [441.0539215686274, 628.318530717958, 628.318530717958])

assert(model.checkPythonDump())
