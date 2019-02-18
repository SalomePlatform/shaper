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

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 =model.addBox(Part_1_doc, 10, 10, 10)
model.end()

from ModelAPI import *
aSession = ModelAPI_Session.get()

aShapeToMove = Box_1.feature().lastResult()

# Verify Rotation errors on low-level
aSession.startOperation()
aRotation = featureToCompositeFeature(Part_1.feature()).addFeature("Rotation")
aRotation.string("CreationMethod").setValue("ByAxisAndAngle")
aRotation.execute()
aSession.finishOperation()
assert(aRotation.error() != "")

aSession.startOperation()
aRotation.selectionList("main_objects").append(aShapeToMove, aShapeToMove.shape())
aRotation.execute()
aSession.finishOperation()
assert(aRotation.error() != "")

aSession.startOperation()
aRotation.selectionList("main_objects").append(None, None)
aRotation.execute()
aSession.finishOperation()
assert(aRotation.error() != "")

aSession.startOperation()
aRotation.string("CreationMethod").setValue("ByThreePoints")
aRotation.execute()
aSession.finishOperation()
assert(aRotation.error() != "")

aSession.startOperation()
aRotation.selectionList("main_objects").removeLast()
aRotation.selectionList("main_objects").removeLast()
aRotation.execute()
aSession.finishOperation()
assert(aRotation.error() != "")

aSession.startOperation()
aRotation.selectionList("main_objects").append(aShapeToMove, aShapeToMove.shape())
aRotation.string("CreationMethod").setValue("ByAxisAndAngle")
aRotation.selection("axis_object").selectSubShape("FACE", "Box_1_1/Back")
aRotation.execute()
aSession.finishOperation()
assert(aRotation.error() != "")

aSession.startOperation()
aRotation.selection("axis_object").selectSubShape("COMPOUND", "Box_1_1/Back")
aRotation.execute()
aSession.finishOperation()
assert(aRotation.error() != "")
