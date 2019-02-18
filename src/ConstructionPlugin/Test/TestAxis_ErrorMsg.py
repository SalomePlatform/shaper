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


# Axis by cylindrical face
aSession.startOperation()
Axis_1 = aPart.addFeature("Axis")
Axis_1.string("CreationMethod").setValue("AxisByCylindricalFaceCase")
Axis_1.selection("CylindricalFace").selectSubShape("COMPOUND", "Compound_2_1")
Axis_1.execute()
aSession.finishOperation()
assert(Axis_1.error() != "")

# Axis by dimensions
aSession.startOperation()
Axis_1.string("CreationMethod").setValue("AxisByDimensionsCase")
Axis_1.real("DX").setValue(0)
Axis_1.real("DY").setValue(0)
Axis_1.real("DZ").setValue(0)
Axis_1.execute()
assert(Axis_1.error() != "")
aSession.finishOperation()

# Axis by line
aSession.startOperation()
Axis_1.string("CreationMethod").setValue("by_line")
Axis_1.selection("line").selectSubShape("COMPOUND", "Compound_1_1")
Axis_1.execute()
assert(Axis_1.error() != "")
aSession.finishOperation()

# Axis by plane and point
aSession.startOperation()
Axis_1.string("CreationMethod").setValue("by_plane_and_point")
Axis_1.selection("point").selectSubShape("COMPOUND", "Compound_1_1")
Axis_1.selection("plane").selectSubShape("COMPOUND", "Compound_2_1")
Axis_1.execute()
assert(Axis_1.error() != "")
aSession.finishOperation()

# Axis by two planes
aSession.startOperation()
Axis_1.string("CreationMethod").setValue("by_two_planes")
Axis_1.selection("plane1").selectSubShape("COMPOUND", "Compound_2_1")
Axis_1.selection("plane2").selectSubShape("COMPOUND", "Compound_2_1")
Axis_1.execute()
assert(Axis_1.error() != "")
aSession.finishOperation()

aSession.startOperation()
Axis_1.selection("plane1").selectSubShape("COMPOUND", "Compound_1_1")
Axis_1.string("use_offset1").setValue("true")
Axis_1.real("offset1").setValue(10)
Axis_1.boolean("reverse_offset1").setValue(True)
Axis_1.execute()
assert(Axis_1.error() != "")
aSession.finishOperation()
