# Copyright (C) 2018-2019  CEA/DEN, EDF R&D
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
from SketchAPI import *
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

#######################################
# Test 1. Make a circle degenerated
#######################################
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(25, 20, 25)
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_1.result(), SketchCircle_1.results()[1])
model.do()
model.end()

# case 1: undo the error
model.begin()
aCoincidence = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).startPoint(), SketchCircle_1.center())
model.do()
model.end()
model.checkSketchErrorDegenerated(Sketch_1)
model.undo()
model.checkSketch(Sketch_1)

# case 2: remove degeneracy-producting constraint
model.begin()
aCoincidence = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).startPoint(), SketchCircle_1.center())
model.do()
model.checkSketchErrorDegenerated(Sketch_1)
Part_1_doc.removeFeature(aCoincidence.feature())
model.do()
model.end()
model.checkSketch(Sketch_1)

# case 3: remove degenerated edge
model.begin()
aCoincidence = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).startPoint(), SketchCircle_1.center())
model.do()
model.checkSketchErrorDegenerated(Sketch_1)
ModelAPI.removeFeaturesAndReferences(FeatureSet([SketchCircle_1.feature()]))
model.do()
model.end()
model.checkSketch(Sketch_1)


#######################################
# Test 2. Make a line degenerated
#######################################
model.begin()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_2 = Sketch_2.addLine(15, 15, 40, 40)
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OX"), True)
SketchLine_3 = SketchProjection_2.createdFeature()
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OY"), True)
SketchLine_4 = SketchProjection_3.createdFeature()
model.do()
model.end()

# case 1: undo the error
model.begin()
aParallel1 = Sketch_2.setParallel(SketchLine_2.result(), SketchLine_3.result())
aParallel2 = Sketch_2.setParallel(SketchLine_2.result(), SketchLine_4.result())
model.do()
model.end()
model.checkSketchErrorDegenerated(Sketch_2)
model.undo()
model.do()
model.checkSketch(Sketch_2)

# case 2: remove degeneracy-producting constraint
model.begin()
aParallel1 = Sketch_2.setParallel(SketchLine_2.result(), SketchLine_3.result())
aParallel2 = Sketch_2.setParallel(SketchLine_2.result(), SketchLine_4.result())
model.do()
model.checkSketchErrorDegenerated(Sketch_2)
Part_1_doc.removeFeature(aParallel1.feature())
Part_1_doc.removeFeature(aParallel2.feature())
model.do()
model.end()
model.checkSketch(Sketch_2)

# case 3: remove degenerated edge
model.begin()
aParallel1 = Sketch_2.setParallel(SketchLine_2.result(), SketchLine_3.result())
aParallel2 = Sketch_2.setParallel(SketchLine_2.result(), SketchLine_4.result())
model.do()
model.checkSketchErrorDegenerated(Sketch_2)
ModelAPI.removeFeaturesAndReferences(FeatureSet([SketchLine_2.feature()]))
model.do()
model.end()
model.checkSketch(Sketch_2)


#######################################
# Test 3. Make an arc degenerated
#######################################
model.begin()
Sketch_3 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchArc_1 = Sketch_3.addArc(-23.95261096318143, -9.48105223640866, -4.386575834490306, -17.91131183048461, -20.45766592507274, 11.53523589116715, False)
model.do()
model.end()

# case 1: undo the error
model.begin()
aCoincidence = Sketch_3.setCoincident(SketchArc_1.startPoint(), SketchArc_1.endPoint())
model.end()
model.checkSketchErrorDegenerated(Sketch_3)
model.undo()
model.begin()
model.checkSketch(Sketch_3)

# case 2: remove degeneracy-producting constraint
model.do()
aCoincidence = Sketch_3.setCoincident(SketchArc_1.startPoint(), SketchArc_1.endPoint())
model.do()
model.checkSketchErrorDegenerated(Sketch_3)
Part_1_doc.removeFeature(aCoincidence.feature())
model.do()
model.checkSketch(Sketch_3)

# case 3: remove degenerated edge
model.do()
aCoincidence = Sketch_3.setCoincident(SketchArc_1.startPoint(), SketchArc_1.endPoint())
model.do()
model.checkSketchErrorDegenerated(Sketch_3)
ModelAPI.removeFeaturesAndReferences(FeatureSet([SketchArc_1.feature()]))
model.end()
assert(Sketch_3.solverError().value() == "")
