## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(2, -40, -30, -40)
SketchLine_2 = Sketch_1.addLine(-30, -40, -30, -5)
SketchLine_3 = Sketch_1.addLine(-30, -5, 2, -5)
SketchLine_4 = Sketch_1.addLine(2, -5, 2, -40)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(40, -25, 2, -25)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_4.result())
SketchLine_6 = Sketch_1.addLine(2, -25, 2, 20)
SketchLine_7 = Sketch_1.addLine(2, 20, 40, 20)
SketchLine_8 = Sketch_1.addLine(40, 20, 40, -25)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 10, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4f-SketchLine_5r-SketchLine_8r-SketchLine_7r-SketchLine_6r")], model.selection(), 10, 0)
Placement_1 = model.addPlacement(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4&weak_name_1][Extrusion_1_1/To_Face]"), model.selection("EDGE", "[Extrusion_2_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_2_1/To_Face]"), False, False)
Placement_2 = model.addPlacement(Part_1_doc, [model.selection("SOLID", "Placement_1_1")], model.selection("VERTEX", "[Placement_1_1/MF:Placed&Sketch_1/SketchLine_4&weak_name_1][Placement_1_1/MF:Placed&Sketch_1/SketchLine_1][Placement_1_1/MF:Placed&Extrusion_1_1/To_Face]"), model.selection("EDGE", "[Extrusion_2_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_2_1/To_Face]"), False, True)
model.do()
model.end()

from GeomAPI import *

# check middle point of the placed solid
shape = Placement_2.results()[0].resultSubShapePair()[0].shape()
assert(shape.middlePoint().distance(GeomAPI_Pnt(-14, 25, 5)) < 1e-7)

# check properties of the result
model.testNbResults(Placement_2, 1)
model.testNbSubResults(Placement_2, [0])
model.testNbSubShapes(Placement_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Placement_2, GeomAPI_Shape.FACE, [7])
model.testNbSubShapes(Placement_2, GeomAPI_Shape.EDGE, [30])
model.testNbSubShapes(Placement_2, GeomAPI_Shape.VERTEX, [60])
model.testResultsVolumes(Placement_2, [11200])
