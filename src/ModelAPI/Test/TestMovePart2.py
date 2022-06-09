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

### Create Part_1 : biggest cylinder
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 50)
Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f")], model.selection(), 10, 0, "Faces|Wires")
model.do()

### Create Plane_4 and Sketch_2, independent
Plane_4 = model.addPlane(partSet, model.selection("FACE", "XOY"), 10, False)
Sketch_2 = model.addSketch(partSet, model.selection("FACE", "Plane_4"))
SketchCircle_2 = Sketch_2.addCircle(0, 0, 20)
model.do()

### Create Part_2, an edge on Sketch_2
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Edge_1 = model.addEdge(Part_2_doc, [model.selection("EDGE", "PartSet/Sketch_1/SketchCircle_1_2")], False)
model.do()

### Create Part_3, small cylinder on a Sketch_2
Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()
Extrusion_2 = model.addExtrusion(Part_3_doc, [model.selection("FACE", "PartSet/Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, 0, "Faces|Wires")
model.do()

### Create Part_4, a box, independent
Part_4 = model.addPart(partSet)
Part_4_doc = Part_4.document()
Sketch_3 = model.addSketch(Part_4_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_3.addLine(70, 60, 20, 60)
SketchLine_2 = Sketch_3.addLine(20, 60, 20, 30)
SketchLine_3 = Sketch_3.addLine(20, 30, 70, 30)
SketchLine_4 = Sketch_3.addLine(70, 30, 70, 60)
Sketch_3.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
Sketch_3.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
Sketch_3.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
Sketch_3.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
Sketch_3.setHorizontal(SketchLine_1.result())
Sketch_3.setVertical(SketchLine_2.result())
Sketch_3.setHorizontal(SketchLine_3.result())
Sketch_3.setVertical(SketchLine_4.result())
model.do()
Extrusion_3 = model.addExtrusion(Part_4_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 10, 0, "Faces|Wires")
model.do()

### Create Placement of Part_4 on the top face of Part_1
Placement_1 = model.addPlacement(partSet, [model.selection("COMPOUND", "Part_4/")], model.selection("FACE", "Part_4/Extrusion_1_1/From_Face"), model.selection("FACE", "Part_1/Extrusion_1_1/To_Face"), keepSubResults = True)

### Create Part_5, revolution
Part_5 = model.addPart(partSet)
Part_5_doc = Part_5.document()
Sketch_4 = model.addSketch(Part_5_doc, model.defaultPlane("XOY"))
SketchCircle_3 = Sketch_4.addCircle(71.54809194299482, -51.10206832217438, 4.892982649360998)
model.do()
Revolution_1 = model.addRevolution(Part_5_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection("EDGE", "PartSet/OX"), -30, 0)
model.do()

### Create Placement_2 of Part_5 revolution to Part_3 to face
Placement_2 = model.addPlacement(partSet, [model.selection("COMPOUND", "Part_5/")], model.selection("FACE", "Part_5/Revolution_1_1/From_Face"), model.selection("FACE", "Part_3/Extrusion_1_1/To_Face"), centering = True, keepSubResults = True)
model.end()

# Check multiple placement of Part_1, Part_3 and Part_5 after theSketch_2
aWhatList = model.FeatureList()
aWhatList.append(Part_1.feature())
aWhatList.append(Part_3.feature())
aWhatList.append(Part_5.feature())
anError = model.validateMovement(SketchCircle_2.feature(), aWhatList).strip()
assert(len(anError) == 0)
# check also that they cannot be moved after Placement_1
anError = model.validateMovement(Placement_1.feature(), aWhatList).strip()
assert(anError == 'Placement_1 -> Part_1')
# do movement
model.do()
partSet.moveFeature(Part_1.feature(), SketchCircle_2.feature())
partSet.moveFeature(Part_3.feature(), Part_1.feature())
partSet.moveFeature(Part_5.feature(), Part_3.feature())
model.end()

# check the resulting list of features
aNames = ['Plane_4', 'Sketch_1', 'Part_1', 'Part_3', 'Part_5', 'Part_2', 'Part_4', 'Placement_1', 'Placement_2']

for aNameIndex in range(9):
  aFeatureName = model.objectToFeature(partSet.object("Features", aNameIndex)).name()
  assert(aNames[aNameIndex] == aFeatureName)
