# Copyright (C) 2021-2026  CEA, EDF
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

import os
from salome.shaper import model

from tempfile import TemporaryDirectory
from ModelAPI import *
from GeomAPI import GeomAPI_Shape
import PrimitivesAPI

model.begin()
partSet = model.moduleDocument()

### Create Part 1
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

### Create Part 2
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Cyl_1 = model.addCylinder(Part_2_doc, 10, 13)

model.end()

model.checkResult(Box_1, model, 1, [0], [1], [6], [24], [48])
model.testResultsVolumes(Box_1, [1000])
model.checkResult(Cyl_1, model, 1, [0], [1], [3],  [6], [12])
model.testResultsVolumes(Cyl_1, [4084.07045])

# Save/load document
with TemporaryDirectory() as tmp_dir:
  aSession = ModelAPI_Session.get()

  # Save
  aFiles = StringList()
  aSession.save(tmp_dir, aFiles)

  # Close and Load again
  aSession.closeAll()
  assert(aSession.load(tmp_dir))

  # Check data
  partSet = aSession.moduleDocument()
  assert(partSet.size("Parts") == 2)

  # Access data of Part 1
  aPart1 = modelAPI_ResultPart(objectToResult(partSet.object("Parts", 0)))
  aSession.startOperation()
  aPart1.activate()
  aSession.finishOperation()
  aPart1Doc = aPart1.partDoc()
  aBoxF = objectToFeature(aPart1Doc.objectByName("Features", "Box_1"))
  aBox = PrimitivesAPI.PrimitivesAPI_Box(aBoxF)
  model.checkResult(aBox, model, 1, [0], [1], [6], [24], [48])
  model.testResultsVolumes(aBox, [1000])

  aSession.startOperation()
  aSession.setActiveDocument(partSet)
  aSession.finishOperation()

  # Access data of Part 2
  aPart2 = modelAPI_ResultPart(objectToResult(partSet.object("Parts", 1)))
  aSession.startOperation()
  aPart2.activate()
  aSession.finishOperation()
  aPart2Doc = aPart2.partDoc()
  aCylF = objectToFeature(aPart2Doc.objectByName("Features", "Cylinder_1"))
  aCyl = PrimitivesAPI.PrimitivesAPI_Cylinder(aCylF)
  model.checkResult(aCyl, model, 1, [0], [1], [3], [6], [12])
  model.testResultsVolumes(aCyl, [4084.07045])
