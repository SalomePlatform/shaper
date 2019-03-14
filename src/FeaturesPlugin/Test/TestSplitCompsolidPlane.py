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
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(11.92861402549826, 0, 16.05621803603458)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.result(), SketchCircle_1.center())
SketchCircle_2 = Sketch_1.addCircle(37.8779560884558, 0, 17.74637067763095)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.result(), SketchCircle_2.center())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Split_1 = model.addSplit(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1_1"), model.selection("SOLID", "Extrusion_1_1_2")], [model.selection("FACE", "PartSet/XOZ")])
model.do()
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Split_1, 1)
model.testNbSubResults(Split_1, [5])
model.testNbSubShapes(Split_1, GeomAPI_Shape.SOLID, [5])
model.testNbSubShapes(Split_1, GeomAPI_Shape.FACE, [25])
model.testNbSubShapes(Split_1, GeomAPI_Shape.EDGE, [90])
model.testNbSubShapes(Split_1, GeomAPI_Shape.VERTEX, [180])
model.testResultsVolumes(Split_1, [16831.622023411000554915517568588])
