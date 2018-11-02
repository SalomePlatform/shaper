## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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

# -*- coding: utf-8 -*-

from salome.shaper import model
from GeomAPI import GeomAPI_Shape

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(50, -50, -50, -50)
SketchLine_2 = Sketch_1.addLine(-50, -50, -50, 50)
SketchLine_3 = Sketch_1.addLine(-50, 50, 50, 50)
SketchLine_4 = Sketch_1.addLine(50, 50, 50, -50)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 100, 0)
Extrusion_1.result().setName("Cube")
Edge_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "[Cube/Generated_Face&Sketch_1/SketchLine_2][Cube/Generated_Face&Sketch_1/SketchLine_1]"), model.selection("EDGE", "[Cube/Generated_Face&Sketch_1/SketchLine_2][Cube/From_Face]"), model.selection("EDGE", "[Cube/Generated_Face&Sketch_1/SketchLine_2][Cube/To_Face]"), model.selection("EDGE", "[Cube/Generated_Face&Sketch_1/SketchLine_3][Cube/Generated_Face&Sketch_1/SketchLine_2]"), model.selection("EDGE", "[Cube/Generated_Face&Sketch_1/SketchLine_1][Cube/From_Face]"), model.selection("EDGE", "[Cube/Generated_Face&Sketch_1/SketchLine_3][Cube/To_Face]"), model.selection("EDGE", "[Cube/Generated_Face&Sketch_1/SketchLine_3][Cube/From_Face]"), model.selection("EDGE", "[Cube/Generated_Face&Sketch_1/SketchLine_1][Cube/To_Face]"), model.selection("EDGE", "[Cube/Generated_Face&Sketch_1/SketchLine_4][Cube/Generated_Face&Sketch_1/SketchLine_1]"), model.selection("EDGE", "[Cube/Generated_Face&Sketch_1/SketchLine_4][Cube/From_Face]"), model.selection("EDGE", "[Cube/Generated_Face&Sketch_1/SketchLine_4][Cube/To_Face]"), model.selection("EDGE", "[Cube/Generated_Face&Sketch_1/SketchLine_4][Cube/Generated_Face&Sketch_1/SketchLine_3]")]
Edge_1 = model.addEdge(Part_1_doc, Edge_1_objects)
# check all the names of Edge_1 results are different
names = set()
for res in Edge_1.results():
    names.add(res.name())
assert(len(names) == len(Edge_1.results())), "Some edges have equal name"

Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_2.addCircle(200, 0, 50)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2r")], model.selection("EDGE", "Cube"), 10, 0)
model.do()
# check Extrusion_2
model.testNbResults(Extrusion_2, 1)
model.testNbSubResults(Extrusion_2, [0])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.EDGE, [6])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.VERTEX, [12])
model.testResultsVolumes(Extrusion_2, [78539.81633974483])

model.end()

assert(model.checkPythonDump())
