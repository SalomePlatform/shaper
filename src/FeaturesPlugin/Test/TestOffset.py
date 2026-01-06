# Copyright (C) 2018-2026  CEA, EDF
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

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Box
Box_1 = model.addBox(Part_1_doc, 200, 200, 200)
Box_2 = model.addBox(Part_1_doc, 200, 200, 200)
Box_3 = model.addBox(Part_1_doc, 200, 200, 200)

### Create Shell
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Box_1_1/Top"),
                                      model.selection("FACE", "Box_1_1/Front"),
                                      model.selection("FACE", "Box_1_1/Left")])

# Global offset (pipe joints)
Offset_1 = model.addOffset(Part_1_doc, model.selection("SHELL", "Shell_1_1"), 70., True)

# Global offset (intersection joints)
Offset_2 = model.addOffset(Part_1_doc, model.selection("SOLID", "Box_2_1"), 60., False)

# Partial offset
Offset_3 = model.addOffsetPartial(Part_1_doc,
                                  model.selection("SOLID", "Box_3_1"),
                                  50.,
                                  [model.selection("FACE", "Box_3_1/Top"),
                                   model.selection("FACE", "Box_3_1/Back"),
                                   model.selection("FACE", "Box_3_1/Right")])

model.end()

from GeomAPI import GeomAPI_Shape

#test Offset_1
model.testNbResults(Offset_1, 1)
model.testNbSubResults(Offset_1, [0])
model.testNbSubShapes(Offset_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Offset_1, GeomAPI_Shape.FACE, [7])
model.testNbSubShapes(Offset_1, GeomAPI_Shape.EDGE, [27])
model.testNbSubShapes(Offset_1, GeomAPI_Shape.VERTEX, [54])
model.testResultsAreas(Offset_1, [193670.3477])

#test Offset_2
model.testNbResults(Offset_1, 1)
model.testNbSubResults(Offset_2, [0])
model.testNbSubShapes(Offset_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Offset_2, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Offset_2, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Offset_2, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Offset_2, [32768000])

#test Offset_3
model.testNbResults(Offset_3, 1)
model.testNbSubResults(Offset_3, [0])
model.testNbSubShapes(Offset_3, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Offset_3, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Offset_3, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Offset_3, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Offset_3, [15625000])
