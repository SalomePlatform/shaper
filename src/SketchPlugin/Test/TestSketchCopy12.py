# Copyright (C) 2020-2021  CEA/DEN, EDF R&D
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

from GeomAPI import *
from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_1 = Sketch_1.addCircle(2, 2, 1)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_1.result(), 2, True)
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_2.result(), 2, True)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 1)
model.do()

Sketch_1_Copy = model.copySketch(Part_1_doc, Sketch_1)

error = model.compareSketches(Sketch_1, Sketch_1_Copy)
assert(error == ""), error

ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 0, 10, [model.selection("SOLID", "Box_1_1")])

Distance = model.lastSubFeature(Sketch_1_Copy, "SketchConstraintDistance")
SketchAPI_Constraint(Distance).setValue(6)
model.do()

ExtrusionCut_2 = model.addExtrusionCut(Part_1_doc, [model.selection("COMPOUND", "Sketch_1_Copy")], model.selection(), 0, 10, [model.selection("SOLID", "ExtrusionCut_1_1")])

model.end()

model.testNbResults(ExtrusionCut_2, 1)
model.testNbSubResults(ExtrusionCut_2, [0])
model.testNbSubShapes(ExtrusionCut_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_2, GeomAPI_Shape.FACE, [8])
model.testNbSubShapes(ExtrusionCut_2, GeomAPI_Shape.EDGE, [36])
model.testNbSubShapes(ExtrusionCut_2, GeomAPI_Shape.VERTEX, [72])
model.testResultsVolumes(ExtrusionCut_2, [937.1681469282])

assert(model.checkPythonDump())
