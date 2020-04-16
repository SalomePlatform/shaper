# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
    Test checks adding sketch into a folder
"""

from ModelAPI import *
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

Sketch_0 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_0.addLine(10, -10, 10, 10)
model.do()

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(50, 50, 25)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2r")], model.selection(), 50, 0)

Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchCircle_2 = Sketch_2.addCircle(100, -100, 50)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_3")], model.selection(), 10, 0)
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 0, 10, [model.selection("SOLID", "Extrusion_2_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_2_1/To_Face_1"))
SketchProjection_1 = Sketch_3.addProjection(model.selection("VERTEX", "[Extrusion_2_1/Generated_Face&Sketch_3/SketchCircle_2_2][Extrusion_2_1/To_Face]__cc"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_3 = Sketch_3.addCircle(100, -100, 25)
SketchConstraintCoincidence_1 = Sketch_3.setCoincident(SketchPoint_1.result(), SketchCircle_3.center())
ExtrusionCut_1.setNestedSketch(Sketch_3)
model.do()
model.end()


aSession = ModelAPI_Session.get()
aPartDoc = aSession.activeDocument()

#=========================================================================
# Test 1. Sketch and extrusion could be added to the folder above
#=========================================================================
aSession.startOperation()
Folder_1 = aPartDoc.addFolder(Sketch_1.feature())
aSession.finishOperation()

NB_FEATURES_FULL = 7
NB_FEATURES_OUT  = 7
# full number of features
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
# number of features outside the folder
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

toFolder = FeatureList()
toFolder.append(Sketch_1.feature())
toFolder.append(Extrusion_1.feature())

# move features to the folder
aSession.startOperation()
aFolder = aPartDoc.findFolderAbove(toFolder)
assert(aFolder is not None and aFolder.data().isEqual(Folder_1.data()))
isAdded = aPartDoc.moveToFolder(toFolder, aFolder)
aSession.finishOperation()
assert(isAdded)
NB_FEATURES_OUT -= 2
# full number of features
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
# number of features outside the folder
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

# check the index of the sketch in the folder
aFound = aPartDoc.findContainingFolder(Sketch_1.feature())
assert(aFound[0].data().isEqual(aFolder.data()))
assert(aFound[1] == 0)
# check the index of the extrusion in the folder
aFound = aPartDoc.findContainingFolder(Extrusion_1.feature())
assert(aFound[0].data().isEqual(aFolder.data()))
assert(aFound[1] == 1)


#=========================================================================
# Test 2. Sketch could be added to the folder below
#=========================================================================
aSession.startOperation()
Folder_2 = aPartDoc.addFolder(Extrusion_2.feature())
aSession.finishOperation()

NB_FEATURES_FULL += 1
NB_FEATURES_OUT  += 1
# full number of features
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
# number of features outside the folder
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

toFolder = FeatureList()
toFolder.append(Sketch_2.feature())

# move feature to the folder
aSession.startOperation()
aFolder = aPartDoc.findFolderBelow(toFolder)
assert(aFolder is not None and aFolder.data().isEqual(Folder_2.data()))
isAdded = aPartDoc.moveToFolder(toFolder, aFolder)
aSession.finishOperation()
assert(isAdded)
NB_FEATURES_OUT -= 1
# full number of features
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
# number of features outside the folder
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

# check the index of the sketch in the folder
aFound = aPartDoc.findContainingFolder(Sketch_2.feature())
assert(aFound[0].data().isEqual(aFolder.data()))
assert(aFound[1] == 0)

# check sketch is the last feature in the folder
aLastFeature = Folder_2.lastVisibleFeature()
assert(aLastFeature.getKind() == "Sketch")


#=========================================================================
# Test 3. Sketch could be removed from the folder
#=========================================================================
fromFolder = FeatureList()
fromFolder.append(Sketch_1.feature())

aSession.startOperation()
isMovedOut = aPartDoc.removeFromFolder(fromFolder)
aSession.finishOperation()
assert(isMovedOut)
assert(aPartDoc.index(Sketch_1.feature(), True) == 1), "Wrong index of the {}: {}".format(Sketch_1.feature().name(), aPartDoc.index(Sketch_1.feature(), True))
NB_FEATURES_OUT += 1
# full number of features
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
# number of features outside the folder
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)


#=========================================================================
# Test 4. Add 2 sketches to the folder below
#=========================================================================

toFolder = FeatureList()
toFolder.append(Sketch_0.feature())
toFolder.append(Sketch_1.feature())

# move features to the folder
aSession.startOperation()
aFolder = aPartDoc.findFolderBelow(toFolder)
assert(aFolder is not None and aFolder.data().isEqual(Folder_1.data()))
isAdded = aPartDoc.moveToFolder(toFolder, aFolder)
aSession.finishOperation()
assert(isAdded)
NB_FEATURES_OUT -= 2
# full number of features
assert(aPartDoc.size("Features") == NB_FEATURES_FULL), "Wrong number of features: {}, expected: {}".format(aPartDoc.size("Features"), NB_FEATURES_FULL)
# number of features outside the folder
assert(aPartDoc.size("Features", True) == NB_FEATURES_OUT), "Wrong number of features outside a folder: {}, expected: {}".format(aPartDoc.size("Features", True), NB_FEATURES_OUT)

# check the index of the sketch in the folder
aFound = aPartDoc.findContainingFolder(Sketch_0.feature())
assert(aFound[0].data().isEqual(aFolder.data()))
assert(aFound[1] == 0)
# check the index of the extrusion in the folder
aFound = aPartDoc.findContainingFolder(Sketch_1.feature())
assert(aFound[0].data().isEqual(aFolder.data()))
assert(aFound[1] == 1)


from salome.shaper import model
assert(model.checkPythonDump())
