# Copyright (C) 2021  CEA/DEN, EDF R&D
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

model.begin()
partSet = model.moduleDocument()

### Create Part 1
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Box_1_1/Left"), model.selection("FACE", "Box_1_1/Top")])

Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Group_2 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Cylinder_1_1/Face_1"), model.selection("FACE", "Box_1_1/Front")])
Group_3 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Cylinder_1_1/Face_1")])
GroupAddition_1 = model.addGroupAddition(Part_1_doc, [model.selection("COMPOUND", "Group_1"), model.selection("COMPOUND", "Group_2")])
GroupSubstraction_1 = model.addGroupSubstraction(Part_1_doc, [model.selection("COMPOUND", "GroupAddition_1")], [model.selection("COMPOUND", "Group_3")])

model.do()

### Create Part 2
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()

### Create ImportResult
ImportResult_1 = model.addImportResult(Part_2_doc, [model.selection("SOLID", "Part_1/Box_1_1")])

model.end()

from GeomAPI import *
from ModelAPI import *
import math

TOLERANCE = 1.e-7

assert(ImportResult_1.feature().error() == "")
model.testNbResults(ImportResult_1, 1)
model.testNbSubResults(ImportResult_1, [0])
model.testNbSubShapes(ImportResult_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ImportResult_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(ImportResult_1, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(ImportResult_1, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(ImportResult_1, [1000])
model.testResultsAreas(ImportResult_1, [600])

# check imported groups are correct
assert(Part_1_doc.size("Groups") == 5)
assert(Part_2_doc.size("Groups") == 4)
REFERENCE = [(2, 5.0, 5.0, 5.0),
             (1, 10.0, 5.0, 5.0),
             (3, 5.0, 5.0, 5.0),
             (3, 5.0, 5.0, 5.0)]
for ind in range(0, Part_2_doc.size("Groups")):
    res = objectToResult(Part_2_doc.object("Groups", ind))
    assert(res is not None)
    nbShapes = 0
    it = GeomAPI_ShapeExplorer(res.shape(), GeomAPI_Shape.FACE)
    while it.more():
        nbShapes += 1
        it.next()
    assert(nbShapes == REFERENCE[ind][0])
    p = res.shape().middlePoint()
    x = REFERENCE[ind][1]
    y = REFERENCE[ind][2]
    z = REFERENCE[ind][3]
    assert(math.fabs(p.x() - x) <= TOLERANCE and math.fabs(p.y() - y) <= TOLERANCE and math.fabs(p.z() - z) <= TOLERANCE), "({}, {}, {}) != ({}, {}, {})".format(p.x(), p.y(), p.z(), x, y, z)

assert(model.checkPythonDump())
