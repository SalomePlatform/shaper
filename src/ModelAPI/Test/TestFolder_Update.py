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

# check the index of the point in the folder
aFound = aPartDoc.findContainingFolder(aPoint1)
assert(aFound[0].data().isEqual(aFolder1.data()))
assert(aFound[1] == 0)

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
aFolder = aPartDoc.findFolderBelow(toFolder)
assert(aFolder is not None)
isAdded = aPartDoc.moveToFolder(toFolder, aFolder)
aSession.finishOperation()
assert(isAdded)

NB_FEATURES_OUT -= 1

assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

# check the index of the point in the folder
aFound = aPartDoc.findContainingFolder(aPoint2)
assert(aFound[0].data().isEqual(aFolder2.data()))
assert(aFound[1] == 0)
aFound = aPartDoc.findContainingFolder(aPoint3)
assert(aFound == -1)

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
aFolder3 = aPartDoc.addFolder(aPoint3)
aSession.finishOperation()

NB_FEATURES_FULL += 1
NB_FEATURES_OUT += 1

assert(aPartDoc.size("Folders") == 3), "Wrong number of folders: {}".format(aPartDoc.size("Folders"))
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

# index without respect to foldering
assert(aPartDoc.index(aFolder3) == 4), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder3))
assert(aPartDoc.index(aPoint4) == 6), "Wrong index of the point: {}".format(aPartDoc.index(aPoint4))
# index according to folders
assert(aPartDoc.index(aFolder3, True) == 2), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder3, True))
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
assert(aPartDoc.index(aFolder3) == 4), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder3))
assert(aPartDoc.index(aPoint3) == 5), "Wrong index of the point: {}".format(aPartDoc.index(aPoint3))
assert(aPartDoc.index(aPoint4) == 6), "Wrong index of the point: {}".format(aPartDoc.index(aPoint4))
# index according to folders
assert(aPartDoc.index(aFolder3, True) == 2), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder3, True))
assert(aPartDoc.index(aPoint3, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint3, True))
assert(aPartDoc.index(aPoint4, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint4, True))

# check the index of the point in the folder
aFound = aPartDoc.findContainingFolder(aPoint3)
assert(aFound[0].data().isEqual(aFolder3.data()))
assert(aFound[1] == 0)
aFound = aPartDoc.findContainingFolder(aPoint4)
assert(aFound[0].data().isEqual(aFolder3.data()))
assert(aFound[1] == 1)


# add more points to the folder to move them out
aPoint5 = newPoint(aPartDoc, 0., 0., 10.)
aPoint6 = newPoint(aPartDoc, 10., 0., 10.)
aPoint7 = newPoint(aPartDoc, 10., 10., 10.)
aPoint8 = newPoint(aPartDoc, 0., 10., 10.)

toFolder = FeatureList()
toFolder.append(aPoint5)
toFolder.append(aPoint6)
toFolder.append(aPoint7)
toFolder.append(aPoint8)

# move point to the folder
aSession.startOperation()
aFolder = aPartDoc.findFolderAbove(toFolder)
assert(aFolder is not None)
isAdded = aPartDoc.moveToFolder(toFolder, aFolder)
aSession.finishOperation()
assert(isAdded)

NB_FEATURES_FULL += 4
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

assert(aPartDoc.index(aPoint5, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint5, True))
assert(aPartDoc.index(aPoint6, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint6, True))
assert(aPartDoc.index(aPoint7, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint7, True))
assert(aPartDoc.index(aPoint8, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint8, True))

assert(aFolder3.reference("first_feature").value() is not None)
assert(aFolder3.reference("last_feature").value() is not None)

#=========================================================================
# Test 4. Remove a point from a folder before it
#=========================================================================
fromFolder = FeatureList()
fromFolder.append(aPoint3)

aSession.startOperation()
isMovedOut = aPartDoc.removeFromFolder(fromFolder)
aSession.finishOperation()
assert(isMovedOut)

NB_FEATURES_OUT += 1
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

assert(aPartDoc.index(aPoint3, True) == 2), "Wrong index of the point: {}".format(aPartDoc.index(aPoint3, True))
assert(aPartDoc.index(aFolder3, True) == 3), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder3, True))

assert(aFolder3.reference("first_feature").value() is not None)
assert(aFolder3.reference("last_feature").value() is not None)

#=========================================================================
# Test 5. Remove a point from a folder after it
#=========================================================================
fromFolder = FeatureList()
fromFolder.append(aPoint8)

aSession.startOperation()
isMovedOut = aPartDoc.removeFromFolder(fromFolder)
aSession.finishOperation()
assert(isMovedOut)

NB_FEATURES_OUT += 1
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

assert(aPartDoc.index(aPoint8, True) == 4), "Wrong index of the point: {}".format(aPartDoc.index(aPoint8, True))
assert(aPartDoc.index(aFolder3, True) == 3), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder3, True))

assert(aFolder3.reference("first_feature").value() is not None)
assert(aFolder3.reference("last_feature").value() is not None)

#=========================================================================
# Test 6. Try to remove several points which are not start nor end in a folder
#=========================================================================
fromFolder = FeatureList()
fromFolder.append(aPoint5)
fromFolder.append(aPoint6)

aSession.startOperation()
isMovedOut = aPartDoc.removeFromFolder(fromFolder)
aSession.finishOperation()
assert(isMovedOut is False)

assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

assert(aPartDoc.index(aFolder3, True) == 3), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder3, True))
assert(aPartDoc.index(aPoint5, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint5, True))
assert(aPartDoc.index(aPoint6, True) == -1), "Wrong index of the point: {}".format(aPartDoc.index(aPoint6, True))
assert(aPartDoc.index(aPoint8, True) == 4), "Wrong index of the point: {}".format(aPartDoc.index(aPoint8, True))

assert(aFolder3.reference("first_feature").value() is not None)
assert(aFolder3.reference("last_feature").value() is not None)

#=========================================================================
# Test 7. Remove several points from a folder after it
#=========================================================================
fromFolder = FeatureList()
fromFolder.append(aPoint6)
fromFolder.append(aPoint7)

aSession.startOperation()
isMovedOut = aPartDoc.removeFromFolder(fromFolder)
aSession.finishOperation()
assert(isMovedOut)

NB_FEATURES_OUT += 2
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

assert(aPartDoc.index(aFolder3, True) == 3), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder3, True))
assert(aPartDoc.index(aPoint6, True) == 4), "Wrong index of the point: {}".format(aPartDoc.index(aPoint6, True))
assert(aPartDoc.index(aPoint7, True) == 5), "Wrong index of the point: {}".format(aPartDoc.index(aPoint7, True))
assert(aPartDoc.index(aPoint8, True) == 6), "Wrong index of the point: {}".format(aPartDoc.index(aPoint8, True))

assert(aFolder3.reference("first_feature").value() is not None)
assert(aFolder3.reference("last_feature").value() is not None)

#=========================================================================
# Test 8. Remove all remaining points from a folder after it
#=========================================================================
fromFolder = FeatureList()
fromFolder.append(aPoint4)
fromFolder.append(aPoint5)

aSession.startOperation()
isMovedOut = aPartDoc.removeFromFolder(fromFolder, False)
aSession.finishOperation()
assert(isMovedOut)

NB_FEATURES_OUT += 2
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

assert(aPartDoc.index(aFolder3, True) == 3), "Wrong index of the folder: {}".format(aPartDoc.index(aFolder3, True))
assert(aPartDoc.index(aPoint4, True) == 4), "Wrong index of the point: {}".format(aPartDoc.index(aPoint4, True))
assert(aPartDoc.index(aPoint5, True) == 5), "Wrong index of the point: {}".format(aPartDoc.index(aPoint5, True))
assert(aPartDoc.index(aPoint6, True) == 6), "Wrong index of the point: {}".format(aPartDoc.index(aPoint6, True))
assert(aPartDoc.index(aPoint7, True) == 7), "Wrong index of the point: {}".format(aPartDoc.index(aPoint7, True))
assert(aPartDoc.index(aPoint8, True) == 8), "Wrong index of the point: {}".format(aPartDoc.index(aPoint8, True))

# folder is empty
assert(aFolder3.reference("first_feature").value() is None)
assert(aFolder3.reference("last_feature").value() is None)


from salome.shaper import model
assert(model.checkPythonDump())
