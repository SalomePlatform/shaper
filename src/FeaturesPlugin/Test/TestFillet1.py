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
from GeomAPI import GeomAPI_Shape

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

Fillet_1 = model.addFillet(Part_1_doc, [model.selection("EDGE", "all-in-Box_1")], 2)
assert(Fillet_1.feature().error() == "")

model.testNbResults(Fillet_1, 1)
model.testNbSubResults(Fillet_1, [0])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.FACE, [26])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.EDGE, [104])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.VERTEX, [208])
model.testResultsVolumes(Fillet_1, [907.704992710141368661425076425])

Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_1 = Sketch_1.addCircle(20, -15, 5)
SketchCircle_2 = Sketch_1.addCircle(30, 10, 3)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)

Fillet_2 = model.addFillet(Part_1_doc, [model.selection("EDGE", "all-in-Extrusion_1")], 2)
assert(Fillet_2.feature().error() != "")

model.end()
