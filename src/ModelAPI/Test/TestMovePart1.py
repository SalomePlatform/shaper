# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

### Create Sketch_1
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 50)
Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
model.do()

### Create Sketch_2 with reference to Sketch_1 circle
Sketch_2 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchCircle_1_2"), False)
SketchCircle_2 = SketchProjection_2.createdFeature()
SketchCircle_3 = Sketch_2.addCircle(34.92852513931564, 34.57180354201876, 20)
Sketch_2.setCoincident(SketchCircle_2.results()[1], SketchCircle_3.center())
model.do()

### Create Part_1: Extrusion of Sketch_1
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "PartSet/Sketch_1/Face-SketchCircle_1_2f")], model.selection(), 10, 0, "Faces|Wires")
model.do()

### Create Part_2: Extrusion of Sketch_2
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Extrusion_2 = model.addExtrusion(Part_2_doc, [model.selection("FACE", "PartSet/Sketch_2/Face-SketchCircle_3_2f")], model.selection(), 20, 0, "Faces|Wires")
model.do()

### Create Placement of Part_2 over a Part_1
Placement_1 = model.addPlacement(partSet, [model.selection("COMPOUND", "Part_2/")], model.selection("FACE", "Part_2/Extrusion_1_1/From_Face"), model.selection("FACE", "Part_1/Extrusion_1_1/To_Face"), centering = True, keepSubResults = True)

### Create independent Part_3
Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()
Sketch_3 = model.addSketch(Part_3_doc, model.defaultPlane("XOY"))
SketchCircle_4 = Sketch_3.addCircle(-60, 65, 10)
model.do()
Extrusion_3 = model.addExtrusion(Part_3_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, 0, "Faces|Wires")
model.do()

model.end()

# test the movement of parts ability and produced errors

# checking ability to move theWhat to position after theTo and produced error theError
def checkMovement(theWhat, theTo, theError):
  aWhatList = model.FeatureList()
  aWhatList.append(theWhat.feature())
  if not theTo is None and not isinstance(theTo, model.ModelAPI_Object):
    theTo = theTo.feature()
  anError = model.validateMovement(theTo, aWhatList).strip()
  aListCheck = theError.split('\n')
  aListGet = anError.split('\n')
  assert(len(aListCheck) == len(aListGet))
  for anErr in aListCheck:
    #print("Looking for " + anErr + " in " + str(aListGet))
    assert(aListGet.count(anErr) == 1)

# movement to very first location actually means movement after the first invisible objects, coordinate system hidden ones
aLastHidden = None
for aFeat in partSet.allObjects():
  if aFeat.isInHistory():
    break
  aLastHidden = model.objectToFeature(aFeat)

checkMovement(Part_1, None, 'Part_1 -> Origin Sketch_1')
checkMovement(Part_1, aLastHidden, 'Part_1 -> Sketch_1')
checkMovement(Part_1, Sketch_1, 'Part_1 -> Sketch_1')
checkMovement(Part_1, Sketch_2, '')
checkMovement(Part_1, Part_2, '')
checkMovement(Part_1, Placement_1, 'Placement_1 -> Part_1')
checkMovement(Part_1, Part_3, 'Placement_1 -> Part_1')

checkMovement(Part_2, None, 'Part_2 -> Origin Sketch_1 Sketch_2')
checkMovement(Part_2, aLastHidden, 'Part_2 -> Sketch_1 Sketch_2')
checkMovement(Part_2, Sketch_1, 'Part_2 -> Sketch_2 Sketch_1') # in GUI this is not the case, because it drops after invisible sketch components
checkMovement(Part_2, Sketch_2, 'Part_2 -> Sketch_2') # in GUI this is not the case, because it drops after invisible sketch components
checkMovement(Part_2, Part_1, '')
checkMovement(Part_2, Placement_1, 'Placement_1 -> Part_2')
checkMovement(Part_2, Part_3, 'Placement_1 -> Part_2')

checkMovement(Part_3, None, '')
checkMovement(Part_3, aLastHidden, '')
checkMovement(Part_3, Sketch_1, '')
checkMovement(Part_3, Sketch_2, '')
checkMovement(Part_3, Part_1, '')
checkMovement(Part_3, Part_2, '')
checkMovement(Part_3, Placement_1, '')

# do movement Part_2 after Sketch_2
anAfterSketch_2 = partSet.nextFeature(Part_1.feature(), True)
model.begin()
partSet.moveFeature(Part_2.feature(), anAfterSketch_2)
model.end()
# check the movement is performed
anAfter = partSet.nextFeature(anAfterSketch_2)
assert(anAfter.name() == 'Part_2')
anAfter = partSet.nextFeature(anAfter)
assert(anAfter.name() == 'Part_1')
anAfter = partSet.nextFeature(anAfter)
assert(anAfter.name() == 'Placement_1')
