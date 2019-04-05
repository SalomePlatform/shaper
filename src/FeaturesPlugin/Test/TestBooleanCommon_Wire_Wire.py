# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-44.32557091040492, 34.37855773243287, -1.296213364851963, -18.43019925529121)
SketchLine_2 = Sketch_1.addLine(-1.296213364851963, -18.43019925529121, 39.1253043294554, 29.81483799275304)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_3 = Sketch_2.addLine(-15.313984501828, -1.226571041729713, -1.296213364851965, -18.43019925529121)
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_1"), False)
SketchLine_4 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_3.startPoint(), SketchLine_4.result())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchAPI_Line(SketchLine_4).endPoint(), SketchLine_3.endPoint())
SketchLine_5 = Sketch_2.addLine(-1.296213364851965, -18.43019925529121, 20.70651786493702, 7.831125115747244)
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_5.startPoint())
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_2"), False)
SketchLine_6 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.result())
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2")])
Wire_2 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchLine_3"), model.selection("EDGE", "Sketch_2/SketchLine_5")])
Common_1 = model.addCommon(Part_1_doc, [model.selection("WIRE", "Wire_1_1"), model.selection("WIRE", "Wire_2_1")])
model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [2])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [4])

assert(model.checkPythonDump())
