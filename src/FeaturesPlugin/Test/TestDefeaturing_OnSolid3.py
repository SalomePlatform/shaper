# Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(10.19197853506727, -21.07109716039953, 30.19197853506727, -21.07109716039953)
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchLine_2 = Sketch_1.addLine(30.19197853506727, -21.07109716039953, 30.19197853506727, -6.071097160399531)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(30.19197853506727, -6.071097160399531, 20.19197853506727, -6.071097160399531)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchLine_4 = Sketch_1.addLine(20.19197853506727, -6.071097160399531, 20.19197853506727, 8.928902839600468)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(20.19197853506727, 8.928902839600468, 30.19197853506727, 8.928902839600468)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchLine_6 = Sketch_1.addLine(30.19197853506727, 8.928902839600468, 30.19197853506727, 18.92890283960047)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(30.19197853506727, 18.92890283960047, 10.19197853506727, 18.92890283960047)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchLine_8 = Sketch_1.addLine(10.19197853506727, 18.92890283960047, 10.19197853506727, -21.07109716039953)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_8.endPoint())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_8.result())
SketchConstraintVertical_3.setName("SketchConstraintVertical_4")
SketchConstraintCollinear_1 = Sketch_1.setCollinear(SketchLine_2.result(), SketchLine_6.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 20)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_8.result(), 40)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_2.result(), 15)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_6.result(), 10)
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_3.result(), 10)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_8f")], model.selection(), 10, 0)
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3"), 0, model.selection(), 0, [model.selection("SOLID", "Extrusion_1_1")])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1"))
SketchLine_9 = Sketch_2.addLine(30.19197853506727, 5, 25.19197853506727, 5)
SketchLine_10 = Sketch_2.addLine(25.19197853506727, 5, 25.19197853506727, 10)
SketchLine_11 = Sketch_2.addLine(25.19197853506727, 10, 30.19197853506727, 10)
SketchLine_12 = Sketch_2.addLine(30.19197853506727, 10, 30.19197853506727, 5)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_10.result())
SketchConstraintVertical_4.setName("SketchConstraintVertical_5")
SketchConstraintHorizontal_6 = Sketch_2.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_12.result())
SketchConstraintVertical_5.setName("SketchConstraintVertical_6")
SketchProjection_1 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/To_Face]"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchPoint_1.result())
SketchConstraintEqual_1 = Sketch_2.setEqual(SketchLine_12.result(), SketchLine_11.result())
SketchConstraintLength_6 = Sketch_2.setLength(SketchLine_12.result(), 5)
ExtrusionCut_1.setNestedSketch(Sketch_2)
Defeaturing_1 = model.addDefeaturing(Part_1_doc, [model.selection("FACE", "ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_10"), model.selection("FACE", "ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9")])
model.testHaveNamingSubshapes(Defeaturing_1, model, Part_1_doc)
model.end()

from GeomAPI import *

model.testNbResults(Defeaturing_1, 1)
model.testNbSubResults(Defeaturing_1, [0])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.FACE, [10])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.EDGE, [48])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.VERTEX, [96])
model.testResultsVolumes(Defeaturing_1, [6500])

assert(model.checkPythonDump())
