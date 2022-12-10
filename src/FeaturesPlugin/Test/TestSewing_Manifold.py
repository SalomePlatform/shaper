# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

aShapeTypes = {
  GeomAPI_Shape.SOLID:  "GeomAPI_Shape.SOLID",
  GeomAPI_Shape.FACE:   "GeomAPI_Shape.FACE",
  GeomAPI_Shape.EDGE:   "GeomAPI_Shape.EDGE",
  GeomAPI_Shape.VERTEX: "GeomAPI_Shape.VERTEX"}

def testNbUniqueSubShapes(theFeature, theShapeType, theExpectedNbSubShapes):
  """ Tests number of unique feature sub-shapes of passed type for each result.
  :param theFeature: feature to test.
  :param theShapeType: shape type of sub-shapes to test.
  :param theExpectedNbSubShapes: list of sub-shapes numbers. Size of list should be equal to len(theFeature.results()).
  """
  aResults = theFeature.feature().results()
  aNbResults = len(aResults)
  aListSize = len(theExpectedNbSubShapes)
  assert (aNbResults == aListSize), "Number of results: {} not equal to list size: {}.".format(aNbResults, aListSize)
  for anIndex in range(0, aNbResults):
    aNbResultSubShapes = 0
    anExpectedNbSubShapes = theExpectedNbSubShapes[anIndex]
    aNbResultSubShapes = aResults[anIndex].shape().subShapes(theShapeType, True).size()
    assert (aNbResultSubShapes == anExpectedNbSubShapes), "Number of sub-shapes of type {} for result[{}]: {}. Expected: {}.".format(aShapeTypes[theShapeType], anIndex, aNbResultSubShapes, anExpectedNbSubShapes)

def testResults(theFeature,theModel,NbRes,NbSubRes,NbShell,NbFace,NbEdge,NbVertex):
  """ Tests numbers of unique sub-shapes in the results
  """
  aResults = theFeature.feature().results()
  aNbResults = len(aResults)
  assert (aNbResults == NbRes), "Number of results: {} not equal to {}}.".format(aNbResults, NbRes)
  theModel.testNbSubResults(theFeature, NbSubRes)
  testNbUniqueSubShapes(theFeature, GeomAPI_Shape.SHELL, NbShell)
  testNbUniqueSubShapes(theFeature, GeomAPI_Shape.FACE, NbFace)
  testNbUniqueSubShapes(theFeature, GeomAPI_Shape.EDGE, NbEdge)
  testNbUniqueSubShapes(theFeature, GeomAPI_Shape.VERTEX, NbVertex)

# Create document
model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Param_dx = model.addParameter(Part_1_doc, "dx", '10')
Param_alfa = model.addParameter(Part_1_doc, "alfa", '90')

# =============================================================================
# Test 1. Sew two shells with a single common edge
# =============================================================================
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], axis = model.selection("EDGE", "PartSet/OX"), distance = "dx", keepSubResults = True)
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Translation_1_1")], axis = model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"), angle = "alfa", keepSubResults = True)

Shell_1_objects = [model.selection("FACE", "Box_1_1/Top"),
                   model.selection("FACE", "Box_1_1/Left"),
                   model.selection("FACE", "Box_1_1/Right"),
                   model.selection("FACE", "Box_1_1/Back"),
                   model.selection("FACE", "Box_1_1/Bottom")]
Shell_1 = model.addShell(Part_1_doc, Shell_1_objects)

Shell_2_objects = [model.selection("FACE", "Rotation_1_1/MF:Rotated&Box_2_1/Top"),
                   model.selection("FACE", "Rotation_1_1/MF:Rotated&Box_2_1/Left"),
                   model.selection("FACE", "Rotation_1_1/MF:Rotated&Box_2_1/Front"),
                   model.selection("FACE", "Rotation_1_1/MF:Rotated&Box_2_1/Right"),
                   model.selection("FACE", "Rotation_1_1/MF:Rotated&Box_2_1/Bottom")]
Shell_2 = model.addShell(Part_1_doc, Shell_2_objects)
model.do()

Sewing_1 = model.addSewing(Part_1_doc, [model.selection("SHELL", "Shell_2_1"), model.selection("SHELL", "Shell_1_1")], 1e-07, allowNonManifold = False, alwaysCreateResult = True)
model.end()

# sewing successful
testResults(Sewing_1, model, 1, [0], [1], [10], [23], [14])

# =============================================================================
# Test 2. Sew two shells with four common edges
# =============================================================================
model.undo()
model.begin()
Param_alfa.setValue(0)
model.do()

Sewing_2 = model.addSewing(Part_1_doc, [model.selection("SHELL", "Shell_2_1"), model.selection("SHELL", "Shell_1_1")], 1e-07, allowNonManifold = False, alwaysCreateResult = True)
model.end()

# sewing successful
testResults(Sewing_2, model, 1, [0], [1], [10], [20], [12])

# =============================================================================
# Test 3. Sew two slightly disconnected shells
# =============================================================================
model.undo()
model.begin()
Param_dx.setValue(10.0001)
model.do()

Sewing_3 = model.addSewing(Part_1_doc, [model.selection("SHELL", "Shell_2_1"), model.selection("SHELL", "Shell_1_1")], 1e-07, allowNonManifold = False, alwaysCreateResult = True)
model.end()

# no sewing done (result is a compound with the two input shells)
testResults(Sewing_3, model, 1, [2], [2], [10], [24], [16])

# =============================================================================
# Test 4. Sew two slightly disconnected shells with larger tolerance
# =============================================================================
model.undo()
model.begin()
Param_dx.setValue(10.0001)
model.do()

Sewing_4 = model.addSewing(Part_1_doc, [model.selection("SHELL", "Shell_2_1"), model.selection("SHELL", "Shell_1_1")], 1e-04, allowNonManifold = False, alwaysCreateResult = True)
model.end()

# sewing successful
testResults(Sewing_4, model, 1, [0], [1], [10], [20], [12])
