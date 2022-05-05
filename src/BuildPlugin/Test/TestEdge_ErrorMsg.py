# Copyright (C) 2017-2022  CEA/DEN, EDF R&D
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

from ModelAPI import *
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

# Create a part
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

# Create auxiliary box
aSession.startOperation()
aBox = aPart.addFeature("Box")
aBox.string("CreationMethod").setValue("BoxByDimensions")
aBox.real("dx").setValue(50)
aBox.real("dy").setValue(50)
aBox.real("dz").setValue(50)
aSession.finishOperation()
aBoxResult = aBox.firstResult()
aBoxShape = aBoxResult.shape()

aSession.startOperation()
Edge_1 = aPart.addFeature("Edge")
Edge_1.execute()
assert(Edge_1.error() != "")
aSession.finishOperation()

aSession.startOperation()
aBaseObjectsList = Edge_1.selectionList("base_objects")
aBaseObjectsList.append("", "EDGE")
Edge_1.execute()
assert(Edge_1.error() != "")
aSession.finishOperation()

aSession.startOperation()
aBaseObjectsList.removeLast()
aBaseObjectsList.append("Box_1_1/Top", "FACE")
Edge_1.execute()
assert(Edge_1.error() != "")
aSession.finishOperation()
