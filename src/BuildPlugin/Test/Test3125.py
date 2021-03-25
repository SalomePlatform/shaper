# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

# Checking of adding a partset-sketcher edges into the wire by the "Add contour" action
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(-24.8768472906404, 33.33497536945814, 36.81773399014779, 23.38423645320196)
SketchLine_2 = Sketch_1.addLine(36.81773399014779, 23.38423645320196, 11.31896551724138, -27.61330049261085)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(11.31896551724138, -27.61330049261085, -24.8768472906404, 33.33497536945814)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
model.do()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Wire_1_objects = [model.selection("EDGE", "PartSet/Sketch_1/SketchLine_3")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects, False)
Wire_1.feature().customAction("add_contour")
model.end()

from ModelAPI import *
from GeomAPI import *

aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Wire_1.feature()))

model.testNbResults(Wire_1, 1)
model.testNbSubShapes(Wire_1, GeomAPI_Shape.EDGE, [3])
model.testNbSubShapes(Wire_1, GeomAPI_Shape.VERTEX, [6])
