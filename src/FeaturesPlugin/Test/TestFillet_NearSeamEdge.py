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
from GeomAPI import GeomAPI_Shape

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

#-----------------------------------------------------------------
model.addParameter(Part_1_doc, "Big_R", '50')
model.addParameter(Part_1_doc, "Big_H", '50')
model.addParameter(Part_1_doc, "Small_R", '5')
model.addParameter(Part_1_doc, "Angle", '10')
#-----------------------------------------------------------------
fillet_radius = model.addParameter(Part_1_doc, "Fillet_R", '4.45')
#-----------------------------------------------------------------

Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "Big_R", "Big_H")
Point_1 = model.addPoint(Part_1_doc, "0", "0", "Big_H/2")
Cylinder_2 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "Point_1"), model.selection("EDGE", "PartSet/OX"), "Small_R", "Big_R*1.25")
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Cylinder_2_1")], axis = model.selection("EDGE", "PartSet/OZ"), angle = "Angle")
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1"), model.selection("SOLID", "Rotation_1_1")])

Fillet_1 = model.addFillet(Part_1_doc, [model.selection("EDGE", "Fuse_1_1/Generated_Edge&Cylinder_2_1/Face_1&Cylinder_1_1/Face_1")], "Fillet_R")
model.do()

assert(Fillet_1.feature().error() == "")
model.testNbResults(Fillet_1, 1)
model.testNbSubResults(Fillet_1, [0])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.EDGE, [16])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.VERTEX, [32])
model.testResultsVolumes(Fillet_1, [393818.53])

fillet_radius.setValue(4.46)
model.do()

assert(Fillet_1.feature().error() == "")
model.testNbResults(Fillet_1, 1)
model.testNbSubResults(Fillet_1, [0])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Fillet_1, [393819.158])

fillet_radius.setValue(4.47)
model.do()

assert(Fillet_1.feature().error() == "")
model.testNbResults(Fillet_1, 1)
model.testNbSubResults(Fillet_1, [0])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Fillet_1, [393819.785])

fillet_radius.setValue(4.48)
model.do()

assert(Fillet_1.feature().error() == "")
model.testNbResults(Fillet_1, 1)
model.testNbSubResults(Fillet_1, [0])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.FACE, [8])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.EDGE, [30])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.VERTEX, [60])
model.testResultsVolumes(Fillet_1, [393820.41])

model.end()
