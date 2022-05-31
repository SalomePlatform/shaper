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

#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *

__updated__ = "2017-11-24"

aSession = ModelAPI_Session.get()


def newPoint(theDocument, theX, theY, theZ):
    aSession.startOperation()
    aPoint = theDocument.addFeature("Point")
    aPointData = aPoint.data()
    assert(aPointData is not None)
    geomDataAPI_Point(aPointData.attribute("point3d")).setValue(theX, theY, theZ)
    aPointData.string("creation_method").setValue("by_xyz")
    aSession.finishOperation()
    return aPoint


#=========================================================================
# Prepare some features and folders
#=========================================================================
aSession.startOperation()
aPart = aSession.moduleDocument().addFeature("Part")
aSession.finishOperation()

# add points
aPartDoc = aSession.activeDocument()
aPoint1 = newPoint(aPartDoc,  0.,  0., 0.)
aPoint2 = newPoint(aPartDoc, 10.,  0., 0.)
aPoint3 = newPoint(aPartDoc, 10., 10., 0.)
aPoint4 = newPoint(aPartDoc,  0., 10., 0.)

# add folders
aSession.startOperation()
aFolder1 = aPartDoc.addFolder(aPoint2)
aSession.finishOperation()
aSession.startOperation()
aFolder2 = aPartDoc.addFolder(aPoint2)
aSession.finishOperation()
aSession.startOperation()
aFolder3 = aPartDoc.addFolder(aPoint3)
aSession.finishOperation()

# place points into folders
toFolder = FeatureList()
toFolder.append(aPoint1)

aSession.startOperation()
aFolder = aPartDoc.findFolderBelow(toFolder)
assert(aFolder is not None)
isAdded = aPartDoc.moveToFolder(toFolder, aFolder)
aSession.finishOperation()
assert(isAdded)

toFolder = FeatureList()
toFolder.append(aPoint3)
toFolder.append(aPoint4)

aSession.startOperation()
aFolder = aPartDoc.findFolderAbove(toFolder)
assert(aFolder is not None)
isAdded = aPartDoc.moveToFolder(toFolder, aFolder)
aSession.finishOperation()
assert(isAdded)

NB_FEATURES_FULL = 7
NB_FEATURES_OUT  = 4

assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

assert(aPartDoc.index(aFolder1, True) == 0), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder1, True))
assert(aPartDoc.index(aFolder2, True) == 1), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder2, True))
assert(aPartDoc.index(aFolder3, True) == 3), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder3, True))

assert(aPartDoc.index(aPoint1, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint1, True))
assert(aPartDoc.index(aPoint2, True) ==  2), "Wrong index of the point: {}".format(aPartDoc.index(aPoint2, True))
assert(aPartDoc.index(aPoint3, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint3, True))
assert(aPartDoc.index(aPoint4, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint4, True))


#=========================================================================
# Test 1. Remove empty folder
#=========================================================================
aSession.startOperation()
aPartDoc.removeFolder(aFolder2)
aSession.finishOperation()

NB_FEATURES_FULL -= 1
NB_FEATURES_OUT  -= 1

assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

assert(aPartDoc.index(aFolder1, True) == 0), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder1, True))
assert(aPartDoc.index(aFolder2, True) == -1), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder2, True))
assert(aPartDoc.index(aFolder3, True) == 2), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder3, True))

assert(aPartDoc.index(aPoint1, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint1, True))
assert(aPartDoc.index(aPoint2, True) ==  1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint2, True))
assert(aPartDoc.index(aPoint3, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint3, True))
assert(aPartDoc.index(aPoint4, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint4, True))


#=========================================================================
# Test 2. Remove non-empty folder
#=========================================================================
aSession.startOperation()
aPartDoc.removeFolder(aFolder3)
aSession.finishOperation()

NB_FEATURES_FULL -= 1
NB_FEATURES_OUT  += 1

assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

assert(aPartDoc.index(aFolder1, True) == 0), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder1, True))
assert(aPartDoc.index(aFolder2, True) == -1), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder2, True))
assert(aPartDoc.index(aFolder3, True) == -1), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder3, True))

assert(aPartDoc.index(aPoint1, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint1, True))
assert(aPartDoc.index(aPoint2, True) ==  1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint2, True))
assert(aPartDoc.index(aPoint3, True) ==  2), "Wrong index of the point: {}".format(aPartDoc.index(aPoint3, True))
assert(aPartDoc.index(aPoint4, True) ==  3), "Wrong index of the point: {}".format(aPartDoc.index(aPoint4, True))


from salome.shaper import model
assert(model.checkPythonDump())
