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

"""
      Unit test of makeVertexInsideFace
"""

from salome.shaper import model
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Sphere
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), 10)

### Get Points Cloud
PC_1 = model.makeVertexInsideFace(Part_1_doc, model.selection("FACE", "Sphere_1_1/Face_1"), 100)

model.end()

### Check result
err = PC_1.feature().error()
if err != "":
    print(err)
    # this test should not fail with old OCCT
    assert("Improper OCCT version" in err)
else:
    model.testNbResults(PC_1, 1)
    model.testNbSubResults(PC_1, [0])
    model.testNbSubShapes(PC_1, GeomAPI_Shape.SOLID, [0])
    model.testNbSubShapes(PC_1, GeomAPI_Shape.FACE, [0])
    model.testNbSubShapes(PC_1, GeomAPI_Shape.EDGE, [0])
    model.testNbSubShapes(PC_1, GeomAPI_Shape.VERTEX, [100])

    assert(model.checkPythonDump())
