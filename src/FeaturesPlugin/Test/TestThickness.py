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
Shell_2 = model.addShell(Part_1_doc, [model.selection("FACE", "Box_2_1/Top"),
                                      model.selection("FACE", "Box_2_1/Front"),
                                      model.selection("FACE", "Box_2_1/Left")])

# Thickness mode 1: thicken a shell/face
Thickness_1 = model.addThickness(Part_1_doc, model.selection("SHELL", "Shell_1_1"), 30, False)
Thickness_2 = model.addThickness(Part_1_doc, model.selection("SHELL", "Shell_2_1"), 30, True)

# Thickness mode 2: hollowed solid
Thickness_3 = model.addHollowedSolid(Part_1_doc,
                                     model.selection("SOLID", "Box_3_1"),
                                     40,
                                     [model.selection("FACE", "Box_3_1/Top"),
                                      model.selection("FACE", "Box_3_1/Left")],
                                     False)

model.end()

from GeomAPI import GeomAPI_Shape

#test Thickness_1
model.testNbResults(Thickness_1, 1)
model.testNbSubResults(Thickness_1, [0])
model.testNbSubShapes(Thickness_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Thickness_1, GeomAPI_Shape.FACE, [12])
model.testNbSubShapes(Thickness_1, GeomAPI_Shape.EDGE, [48])
model.testNbSubShapes(Thickness_1, GeomAPI_Shape.VERTEX, [96])
model.testResultsVolumes(Thickness_1, [4167000])

#test Thickness_2
model.testNbResults(Thickness_1, 1)
model.testNbSubResults(Thickness_2, [0])
model.testNbSubShapes(Thickness_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Thickness_2, GeomAPI_Shape.FACE, [12])
model.testNbSubShapes(Thickness_2, GeomAPI_Shape.EDGE, [48])
model.testNbSubShapes(Thickness_2, GeomAPI_Shape.VERTEX, [96])
model.testResultsVolumes(Thickness_2, [3087000])

#test Thickness_3
model.testNbResults(Thickness_3, 1)
model.testNbSubResults(Thickness_3, [0])
model.testNbSubShapes(Thickness_3, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Thickness_3, GeomAPI_Shape.FACE, [10])
model.testNbSubShapes(Thickness_3, GeomAPI_Shape.EDGE, [48])
model.testNbSubShapes(Thickness_3, GeomAPI_Shape.VERTEX, [96])
model.testResultsVolumes(Thickness_3, [8128000])
