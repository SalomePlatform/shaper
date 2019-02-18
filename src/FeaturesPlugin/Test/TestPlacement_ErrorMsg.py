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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-16.57843122838298, -30.87163595652513, 16.88033171559421)
SketchCircle_2 = Sketch_1.addCircle(29.74848722752363, 33.32178840955915, 9.026354269133289)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
model.do()
model.end()

from ModelAPI import *
aSession = ModelAPI_Session.get()

aShapeToMove = Extrusion_1.feature().results()[1]

# Verify Placement errors on low-level
aSession.startOperation()
aPlacementFt = featureToCompositeFeature(Part_1.feature()).addFeature("Placement")
aPlacementFt.execute()
aSession.finishOperation()
assert(aPlacementFt.error() != "")

aSession.startOperation()
aPlacementFt.selectionList("placement_objects_list").append(aShapeToMove, aShapeToMove.shape())
aPlacementFt.execute()
aSession.finishOperation()
assert(aPlacementFt.error() != "")

aSession.startOperation()
aPlacementFt.selection("placement_start_shape").selectSubShape("FACE", "Extrusion_1_2/To_Face")
aPlacementFt.execute()
aSession.finishOperation()
assert(aPlacementFt.error() != "")

aSession.startOperation()
aPlacementFt.selection("placement_start_shape").selectSubShape("FACE", "Extrusion_1_2/Generated_Face&Sketch_1/SketchCircle_2_2")
aPlacementFt.selection("placement_end_shape").selectSubShape("FACE", "Extrusion_1_1/To_Face")
aPlacementFt.execute()
aSession.finishOperation()
assert(aPlacementFt.error() != "")

aSession.startOperation()
aPlacementFt.selection("placement_start_shape").selectSubShape("EDGE", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchCircle_2_2][Extrusion_1_2/To_Face]")
aPlacementFt.selection("placement_end_shape").selectSubShape("FACE", "Extrusion_1_1/To_Face")
aPlacementFt.execute()
aSession.finishOperation()
assert(aPlacementFt.error() != "")

# Create correct Placement
aSession.startOperation()
aPlacementFt.selection("placement_start_shape").selectSubShape("FACE", "Extrusion_1_2/To_Face")
aPlacementFt.selection("placement_end_shape").selectSubShape("FACE", "Extrusion_1_1/To_Face")
aPlacementFt.boolean("placement_reverse_direction").setValue(False)
aPlacementFt.boolean("placement_centering").setValue(True)
aPlacementFt.execute()
aSession.finishOperation()
assert(aPlacementFt.error() == "")


model.begin()
Placement_2 = model.addPlacement(Part_1_doc, [model.selection("SOLID", "Placement_1_1")], model.selection("COMPOUND", "Sketch_1"), model.selection("FACE", "Extrusion_1_1/To_Face"), False, False)
assert(Placement_2.feature().error() != "")
Part_1_doc.removeFeature(Placement_2.feature())
model.end()
