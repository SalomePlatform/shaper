# Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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
aBoxResult = aBox.firstResult()
aBoxShape = aBoxResult.shape()

aSession.startOperation()
Wire_1 = aPart.addFeature("Wire")
Wire_1.execute()
assert(Wire_1.error() != "")
aSession.finishOperation()

aSession.startOperation()
aBaseObjectsList = Wire_1.selectionList("base_objects")
aBaseObjectsList.append("[Box_1_1/Left][Box_1_1/Top]", "EDGE")
aBaseObjectsList.append("[Box_1_1/Right][Box_1_1/Top]", "EDGE")
Wire_1.execute()
assert(Wire_1.error() != "")
aSession.finishOperation()


from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

Wire_1 = model.addWire(Part_1_doc, [])
Wire_1.addContour()
Part_1_doc.removeFeature(Wire_1.feature())

Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]")])
Wire_1.addContour()
Part_1_doc.removeFeature(Wire_1.feature())

Wire_1 = model.addWire(Part_1_doc, [model.selection("FACE", "Box_1_1/Top")])
Wire_1.addContour()
Part_1_doc.removeFeature(Wire_1.feature())

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(26.2037104714363, 14.67693246486999, 44.63854591961698, 30.23007341140914)
SketchLine_2 = Sketch_1.addLine(44.63854591961698, 30.23007341140914, 20.93564110678636, 53.45762191102688)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
model.do()

Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1")])
Wire_1.addContour()
Part_1_doc.removeFeature(Wire_1.feature())

model.end()
