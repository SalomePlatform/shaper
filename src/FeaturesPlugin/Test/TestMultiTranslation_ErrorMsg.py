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
aCompound.selectionList("base_objects").append("Box_1_1", "SOLID")
aSession.finishOperation()


aSession.startOperation()
aLinearCopy1 = aPart.addFeature("LinearCopy")
aLinearCopy1.execute()
assert(aLinearCopy1.error() != "")
aSession.finishOperation()

aSession.startOperation()
aLinearCopy1.selectionList("main_objects").append("Compound_1_1", "COMPOUND")
aLinearCopy1.execute()
assert(aLinearCopy1.error() != "")
aSession.finishOperation()

aSession.startOperation()
aLinearCopy1.selection("axis_first_dir").selectSubShape("COMPOUND", "Compound_1_1")
aLinearCopy1.execute()
assert(aLinearCopy1.error() != "")
aSession.finishOperation()

aSession.startOperation()
aLinearCopy1.selection("axis_first_dir").selectSubShape("EDGE", "PartSet/OX")
aLinearCopy1.integer("nb_first_dir").setValue(2)
aLinearCopy1.execute()
assert(aLinearCopy1.error() != "")
aSession.finishOperation()


aSession.startOperation()
aLinearCopy2 = aPart.addFeature("LinearCopy")
aLinearCopy2.string("use_second_dir").setValue("true")
aLinearCopy2.execute()
assert(aLinearCopy2.error() != "")
aSession.finishOperation()

aSession.startOperation()
aLinearCopy2.selectionList("main_objects").append("Compound_1_1", "COMPOUND")
aLinearCopy2.execute()
assert(aLinearCopy2.error() != "")
aSession.finishOperation()

aSession.startOperation()
aLinearCopy2.selection("axis_first_dir").selectSubShape("COMPOUND", "Compound_1_1")
aLinearCopy2.execute()
assert(aLinearCopy2.error() != "")
aSession.finishOperation()

aSession.startOperation()
aLinearCopy2.selection("axis_first_dir").selectSubShape("EDGE", "PartSet/OX")
aLinearCopy2.selection("axis_second_dir").selectSubShape("COMPOUND", "Compound_1_1")
aLinearCopy2.execute()
assert(aLinearCopy2.error() != "")
aSession.finishOperation()

aSession.startOperation()
aLinearCopy2.selection("axis_second_dir").selectSubShape("EDGE", "PartSet/OY")
aLinearCopy2.integer("nb_first_dir").setValue(0)
aLinearCopy2.integer("nb_second_dir").setValue(0)
aLinearCopy2.execute()
assert(aLinearCopy2.error() != "")
aSession.finishOperation()

aSession.startOperation()
aLinearCopy2.integer("nb_first_dir").setValue(2)
aLinearCopy2.integer("nb_second_dir").setValue(2)
aLinearCopy2.execute()
assert(aLinearCopy2.error() != "")
aSession.finishOperation()
