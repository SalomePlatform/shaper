## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *

__updated__ = "2017-11-23"

aSession = ModelAPI_Session.get()


def newPoint(theDocument, theX, theY, theZ):
    aSession.startOperation()
    aPoint = theDocument.addFeature("Point")
    aPointData = aPoint.data()
    assert(aPointData is not None)
    aPointData.real("x").setValue(theX)
    aPointData.real("y").setValue(theY)
    aPointData.real("z").setValue(theZ)
    aPointData.string("creation_method").setValue("by_xyz")
    aSession.finishOperation()
    return aPoint


#=========================================================================
# Test 1. Add a point into a folder above
#=========================================================================
aSession.startOperation()
aPart = aSession.moduleDocument().addFeature("Part")
aSession.finishOperation()

# add point and a folder before it
aPartDoc = aSession.activeDocument()
aPoint1 = newPoint(aPartDoc, 0., 0., 0.)

aSession.startOperation()
aFolder1 = aPartDoc.addFolder(aPoint1)
aSession.finishOperation()

NB_FEATURES_FULL = 2
NB_FEATURES_OUT  = 2

assert(aPartDoc.size("Folders") == 1), "Wrong number of folders: {}".format(aPartDoc.size("Folders"))
assert(aPartDoc.size("Features") == 2), "Wrong number of features: {}".format(aPartDoc.size("Features"))
FOLDER_NAME_EXPECTED = "Folder_1"
assert(aFolder1.name() == FOLDER_NAME_EXPECTED), "Actual name '{}', expected '{}'".format(aFolder1.name(), FOLDER_NAME_EXPECTED)

toFolder = FeatureList()
toFolder.append(aPoint1)

# move point to the folder
aSession.startOperation()
aFolder = aPartDoc.findFolderAbove(toFolder)
assert(aFolder is not None)
isAdded = aPartDoc.moveToFolder(toFolder, aFolder)
aSession.finishOperation()
assert(isAdded)
NB_FEATURES_OUT -= 1
# full number of features
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
# number of features outside the folder
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

#=========================================================================
# Test 2. Add a point into a folder below
#=========================================================================
aPoint2 = newPoint(aPartDoc, 10., 0., 0.)
aPoint3 = newPoint(aPartDoc, 10., 10., 0.)

NB_FEATURES_FULL += 2
NB_FEATURES_OUT += 2

assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

# add a folder
aSession.startOperation()
aFolder2 = aPartDoc.addFolder(aPoint3)
aSession.finishOperation()

NB_FEATURES_FULL += 1
NB_FEATURES_OUT += 1

assert(aPartDoc.size("Folders") == 2), "Wrong number of folders: {}".format(aPartDoc.size("Folders"))
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

toFolder = FeatureList()
toFolder.append(aPoint2)

# move point to the folder
aSession.startOperation()
aFolder = aPartDoc.findFolderAbove(toFolder)
assert(aFolder is not None)
isAdded = aPartDoc.moveToFolder(toFolder, aFolder)
aSession.finishOperation()
assert(isAdded)

NB_FEATURES_OUT -= 1

assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

#=========================================================================
# Test 3. Add several points into a folder
#=========================================================================
aPoint4 = newPoint(aPartDoc, 0., 10., 0.)

NB_FEATURES_FULL += 1
NB_FEATURES_OUT += 1

assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

# index without respect to foldering
assert(aPartDoc.index(aPoint4) == 5), "Wrong index of the point: {}".format(aPartDoc.index(aPoint4))
# index according to folders
assert(aPartDoc.index(aPoint4, True) == 3), "Wrong index of the point: {}".format(aPartDoc.index(aPoint4, True))

# add a folder
aSession.startOperation()
aFolder2 = aPartDoc.addFolder(aPoint3)
aSession.finishOperation()

NB_FEATURES_FULL += 1
NB_FEATURES_OUT += 1

assert(aPartDoc.size("Folders") == 3), "Wrong number of folders: {}".format(aPartDoc.size("Folders"))
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

# index without respect to foldering
assert(aPartDoc.index(aFolder2) == 4), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder2))
assert(aPartDoc.index(aPoint4) == 6), "Wrong index of the point: {}".format(aPartDoc.index(aPoint4))
# index according to folders
assert(aPartDoc.index(aFolder2, True) == 2), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder2, True))
assert(aPartDoc.index(aPoint4, True) == 4), "Wrong index of the point: {}".format(aPartDoc.index(aPoint4, True))

toFolder = FeatureList()
toFolder.append(aPoint3)
toFolder.append(aPoint4)

# move point to the folder
aSession.startOperation()
aFolder = aPartDoc.findFolderAbove(toFolder)
assert(aFolder is not None)
isAdded = aPartDoc.moveToFolder(toFolder, aFolder)
aSession.finishOperation()
assert(isAdded)

NB_FEATURES_OUT -= 2

assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

# index without respect to foldering
assert(aPartDoc.index(aFolder2) == 4), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder2))
assert(aPartDoc.index(aPoint3) == 5), "Wrong index of the point: {}".format(aPartDoc.index(aPoint3))
assert(aPartDoc.index(aPoint4) == 6), "Wrong index of the point: {}".format(aPartDoc.index(aPoint4))
# index according to folders
assert(aPartDoc.index(aFolder2, True) == 2), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder2, True))
assert(aPartDoc.index(aPoint3, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint3, True))
assert(aPartDoc.index(aPoint4, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint4, True))


from salome.shaper import model
assert(model.checkPythonDump())
