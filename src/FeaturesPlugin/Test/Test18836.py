# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(141.7325102880659, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 87.1454046639232)
SketchLine_3 = Sketch_1.addLine(0, 87.1454046639232, 141.7325102880659, 87.1454046639232)
SketchLine_4 = Sketch_1.addLine(141.7325102880659, 87.1454046639232, 141.7325102880659, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(48.25918560939968, 36.68732867484031, -63.06551064348577, 36.68732867484031)
SketchLine_6 = Sketch_1.addLine(-63.06551064348577, 36.68732867484031, -63.06551064348577, -40.64885412030576)
SketchLine_7 = Sketch_1.addLine(-63.06551064348577, -40.64885412030576, 48.25918560939968, -40.64885412030576)
SketchLine_8 = Sketch_1.addLine(48.25918560939968, -40.64885412030576, 48.25918560939968, 36.68732867484031)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
SketchLine_9 = Sketch_1.addLine(-12.82162118421888, 83.48310807317709, -70.45431791690731, 83.48310807317709)
SketchLine_10 = Sketch_1.addLine(-70.45431791690731, 83.48310807317709, -70.45431791690731, 46.53907170606908)
SketchLine_11 = Sketch_1.addLine(-70.45431791690731, 46.53907170606908, -12.82162118421888, 46.53907170606908)
SketchLine_12 = Sketch_1.addLine(-12.82162118421888, 46.53907170606908, -12.82162118421888, 83.48310807317709)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_10.result())
SketchConstraintHorizontal_6 = Sketch_1.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_12.result())
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], True)
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Wire_1")], model.selection("EDGE", "PartSet/OZ"), 10, 0)
Edge_1 = model.addEdge(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], True)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Edge_1")], model.selection("EDGE", "PartSet/OZ"), -10, 0)
model.end()

from GeomAPI import *

model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [3])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [16])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [64])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [128])
model.testResultsAreas(Extrusion_1, [10242.31054])

model.testNbResults(Extrusion_2, 1)
model.testNbSubResults(Extrusion_2, [16])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.FACE, [16])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.EDGE, [64])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.VERTEX, [128])
model.testResultsAreas(Extrusion_2, [10242.31054])

assert(model.checkPythonDump())