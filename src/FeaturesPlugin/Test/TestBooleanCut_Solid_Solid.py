# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(15.58517034068135, 30.56312625250501, -33.39679358717434, 30.56312625250501)
SketchLine_2 = Sketch_1.addLine(-33.39679358717434, 30.56312625250501, -33.39679358717434, -16.39478957915832)
SketchLine_3 = Sketch_1.addLine(-33.39679358717434, -16.39478957915832, 15.58517034068135, -16.39478957915832)
SketchLine_4 = Sketch_1.addLine(15.58517034068135, -16.39478957915832, 15.58517034068135, 30.56312625250501)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_5 = Sketch_2.addLine(35.01603206412826, 45.13627254509017, -8.298597194388776, 45.13627254509017)
SketchLine_6 = Sketch_2.addLine(-8.298597194388776, 45.13627254509017, -8.298597194388776, 6.679358717434865)
SketchLine_7 = Sketch_2.addLine(-8.298597194388776, 6.679358717434865, 35.01603206412826, 6.679358717434865)
SketchLine_8 = Sketch_2.addLine(35.01603206412826, 6.679358717434865, 35.01603206412826, 45.13627254509017)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_8.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1"), model.selection("COMPOUND", "Sketch_2")], model.selection(), 100, 0)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], [model.selection("SOLID", "Extrusion_1_2")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [8])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [36])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [72])

assert(model.checkPythonDump())
