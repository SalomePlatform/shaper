# Copyright (C) 2020-2021  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(5, 1.355252715607035e-20, 20, -1.336382355046098e-51)
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchLine_2 = Sketch_1.addLine(20, -1.336382355046098e-51, 20, 5.85786437626905)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(20, 5.85786437626905, 15.85786437626905, 10)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(15.85786437626905, 10, 20, 14.14213562373095)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(20, 14.14213562373095, 20, 20)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(20, 20, 0, 20)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchLine_7 = Sketch_1.addLine(0, 20, 0, 5)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintVertical_2.setName("SketchConstraintVertical_3")
SketchArc_1 = Sketch_1.addArc(0, 0, 5, 1.355252715607035e-20, 0, 5, False)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchArc_1.endPoint())
SketchConstraintCollinear_1 = Sketch_1.setCollinear(SketchLine_2.result(), SketchLine_5.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_3.result(), SketchLine_4.result())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_1.result())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_7.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_1.center(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_1.center(), SketchLine_1.endPoint(), 20, True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchArc_1.center(), SketchLine_7.startPoint(), 20, True)
SketchLine_8 = Sketch_1.addLine(10, 0, 10, 20)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_8.startPoint(), SketchLine_1.result())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_6.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_8.result())
SketchConstraintVertical_3.setName("SketchConstraintVertical_4")
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchLine_8.endPoint(), SketchLine_6.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_2.result(), SketchLine_5.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_4.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 5)
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_2.result(), SketchLine_3.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Defeaturing_1 = model.addDefeaturing(Part_1_doc, [model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchArc_1_2"), model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_3")])
model.testHaveNamingSubshapes(Defeaturing_1, model, Part_1_doc)
model.end()

from GeomAPI import *

model.testNbResults(Defeaturing_1, 1)
model.testNbSubResults(Defeaturing_1, [2])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.FACE, [14])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.EDGE, [60])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.VERTEX, [120])
model.testResultsVolumes(Defeaturing_1, [4000])

assert(model.checkPythonDump())
