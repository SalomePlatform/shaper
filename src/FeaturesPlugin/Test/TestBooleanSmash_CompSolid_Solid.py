# Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-0.820365148699405, 7.40718257641344, 6.028904154999336)
SketchCircle_2 = Sketch_1.addCircle(10.62009910116713, 13.94078752172051, 10.99163472554975)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Smash_1 = model.addSmash(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1_1"), model.selection("SOLID", "Extrusion_1_1_2")], [model.selection("SOLID", "Box_1_1")])
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Smash_1, 1)
model.testNbSubResults(Smash_1, [4])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.SOLID, [4])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.FACE, [28])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.EDGE, [120])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.VERTEX, [240])
model.testResultsVolumes(Smash_1, [4980.404017506245509139262139797])
