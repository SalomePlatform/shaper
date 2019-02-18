# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *

__updated__ = "2017-11-22"

#=========================================================================
# Test 1. Check the Folder is created in PartSet
#=========================================================================
aSession = ModelAPI_Session.get()
aPartSetDoc = aSession.activeDocument()

aSession.startOperation()
aPoint0 = aPartSetDoc.addFeature("Point")
aPoint0Data = aPoint0.data()
assert(aPoint0Data is not None)
geomDataAPI_Point(aPoint0Data.attribute("point3d")).setValue(0., 0., 0.)
aPoint0Data.string("creation_method").setValue("by_xyz")
aSession.finishOperation()

aSession.startOperation()
aPoint1 = aPartSetDoc.addFeature("Point")
aPoint1Data = aPoint1.data()
assert(aPoint1Data is not None)
geomDataAPI_Point(aPoint1Data.attribute("point3d")).setValue(0., 0., 0.)
aPoint1Data.string("creation_method").setValue("by_xyz")
aSession.finishOperation()

assert(aPartSetDoc.size("Features") == 2), "Wrong number of features: {}".format(aPartSetDoc.size("Features"))

# Folder before the feature
aSession.startOperation()
aFolder1 = aPartSetDoc.addFolder(aPoint1)
aSession.finishOperation()

assert(aPartSetDoc.size("Folders") == 1), "Wrong number of folders: {}".format(aPartSetDoc.size("Folders"))
assert(aPartSetDoc.size("Features") == 3), "Wrong number of features: {}".format(aPartSetDoc.size("Features"))
FOLDER_NAME_EXPECTED = "Folder_1"
assert(aFolder1.name() == FOLDER_NAME_EXPECTED), "Actual name '{}', expected '{}'".format(aFolder1.name(), FOLDER_NAME_EXPECTED)

## Folder at the end of features list
#aSession.startOperation()
#aPartSetDoc.addFolder()
#aSession.finishOperation()
#assert(aPartSetDoc.size("Folders") == 2)

#=========================================================================
# Test 2. Check the Folder is created in a Part
#=========================================================================
aSession.startOperation()
aPart = aSession.moduleDocument().addFeature("Part")
aSession.finishOperation()

# check part is a last feature
aFeaturesList = aSession.moduleDocument().allObjects()
aLast = aFeaturesList[len(aFeaturesList)-1]
assert(aLast.data().isEqual(aPart.data())), "Part is not a last object in the list"

# add point and a folder before it
aPartDoc = aSession.activeDocument()
aSession.startOperation()
aPoint2 = aPartDoc.addFeature("Point")
aPoint2Data = aPoint2.data()
assert(aPoint2Data is not None)
geomDataAPI_Point(aPoint2Data.attribute("point3d")).setValue(0., 0., 0.)
aPoint2Data.string("creation_method").setValue("by_xyz")
aSession.finishOperation()

assert(aPartDoc.size("Features") == 1), "Wrong number of features: {}".format(aPartDoc.size("Features"))

aSession.startOperation()
aFolder2 = aPartDoc.addFolder(aPoint2)
aSession.finishOperation()

assert(aPartDoc.size("Folders") == 1), "Wrong number of folders: {}".format(aPartDoc.size("Folders"))
assert(aPartDoc.size("Features") == 2), "Wrong number of features: {}".format(aPartDoc.size("Features"))
FOLDER_NAME_EXPECTED = "Folder_1"
assert(aFolder2.name() == FOLDER_NAME_EXPECTED), "Actual name '{}', expected '{}'".format(aFolder2.name(), FOLDER_NAME_EXPECTED)
