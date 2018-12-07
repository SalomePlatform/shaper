## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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


# Point by line and plane
aSession.startOperation()
Point_1 = aPart.addFeature("Point")
Point_1.string("creation_method").setValue("by_intersection")
Point_1.string("intersection_type").setValue("intersection_type_by_line_and_plane")
Point_1.selection("intersection_line").selectSubShape("COMPOUND", "Compound_1_1")
Point_1.selection("intersection_plane").selectSubShape("COMPOUND", "Compound_2_1")
Point_1.execute()
assert(Point_1.error() != "")
aSession.finishOperation()
