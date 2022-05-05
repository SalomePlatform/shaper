# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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

# Create auxiliary compound
aSession.startOperation()
aCompound = aPart.addFeature("Compound")
aCompoundList = aCompound.selectionList("base_objects")
aCompoundList.append("Box_1_1/Front", "FACE")
aCompoundList.append("[Box_1_1/Left][Box_1_1/Top]", "EDGE")
aSession.finishOperation()

# Create auxiliary box 2
aSession.startOperation()
aBox = aPart.addFeature("Box")
aBox.string("CreationMethod").setValue("BoxByDimensions")
aBox.real("dx").setValue(50)
aBox.real("dy").setValue(50)
aBox.real("dz").setValue(50)
aSession.finishOperation()

# Create auxiliary compound 2
aSession.startOperation()
aCompound = aPart.addFeature("Compound")
aCompoundList = aCompound.selectionList("base_objects")
aCompoundList.append("[Box_2_1/Left][Box_2_1/Top]", "EDGE")
aCompoundList.append("Box_2_1/Front", "FACE")
aSession.finishOperation()


# Plane by line and point
aSession.startOperation()
Plane_1 = aPart.addFeature("Plane")
Plane_1.string("creation_method").setValue("by_line_and_point")
Plane_1.selection("line").selectSubShape("COMPOUND", "Compound_1_1")
Plane_1.selection("point").selectSubShape("VERTEX", "PartSet/Origin")
Plane_1.execute()
assert(Plane_1.error() != "")
aSession.finishOperation()

# Unsupported type
aSession.startOperation()
Plane_1.string("creation_method").setValue("by_rotation")
Plane_1.selection("plane").selectSubShape("COMPOUND", "Compound_1_1")
Plane_1.selection("axis").selectSubShape("COMPOUND", "Compound_1_1")
Plane_1.execute()
assert(Plane_1.error() != "")
aSession.finishOperation()

# By two parallel planes
aSession.startOperation()
Plane_1.string("creation_method").setValue("by_two_parallel_planes")
Plane_1.selection("plane1").selectSubShape("COMPOUND", "Compound_2_1")
Plane_1.selection("plane2").selectSubShape("COMPOUND", "Compound_2_1")
Plane_1.execute()
assert(Plane_1.error() != "")
aSession.finishOperation()

aSession.startOperation()
Plane_1.selection("plane1").selectSubShape("COMPOUND", "Compound_1_1")
Plane_1.execute()
assert(Plane_1.error() != "")
aSession.finishOperation()

# By distance from other plane
aSession.startOperation()
Plane_1.string("creation_method").setValue("by_other_plane")
Plane_1.string("by_other_plane_option").setValue("by_distance_from_other")
Plane_1.selection("plane").selectSubShape("COMPOUND", "Compound_2_1")
Plane_1.real("distance").setValue(10)
Plane_1.execute()
assert(Plane_1.error() != "")
aSession.finishOperation()

# By coincident point
aSession.startOperation()
Plane_1.string("by_other_plane_option").setValue("by_coincident_to_point")
Plane_1.selection("coincident_point").selectSubShape("COMPOUND", "Compound_1_1")
Plane_1.execute()
assert(Plane_1.error() != "")
aSession.finishOperation()

# By rotation
aSession.startOperation()
Plane_1.string("by_other_plane_option").setValue("by_rotation")
Plane_1.selection("axis").selectSubShape("COMPOUND", "Compound_1_1")
Plane_1.real("angle").setValue(90)
Plane_1.execute()
assert(Plane_1.error() != "")
aSession.finishOperation()

aSession.startOperation()
Plane_1.selection("plane").selectSubShape("COMPOUND", "Compound_1_1")
Plane_1.execute()
assert(Plane_1.error() != "")
aSession.finishOperation()
