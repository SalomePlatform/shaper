## Copyright (C) 2019-20xx  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(68.30992366412212, 32.22748091603054, -50.73129770992366, 32.22748091603054)
SketchLine_2 = Sketch_1.addLine(-50.73129770992366, 32.22748091603054, -50.73129770992366, -43.02137404580152)
SketchLine_3 = Sketch_1.addLine(-50.73129770992366, -43.02137404580152, 68.30992366412212, -43.02137404580152)
SketchLine_4 = Sketch_1.addLine(68.30992366412212, -43.02137404580152, 68.30992366412212, 32.22748091603054)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Vertex_1_objects = [model.selection("VERTEX", "Sketch_1/SketchLine_2_StartVertex"), model.selection("VERTEX", "Sketch_1/SketchLine_3_StartVertex"), model.selection("VERTEX", "Sketch_1/SketchLine_3_EndVertex"), model.selection("VERTEX", "Sketch_1/SketchLine_4_EndVertex")]
Vertex_1 = model.addVertex(Part_1_doc, Vertex_1_objects)
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Vertex_1")], model.selection("EDGE", "PartSet/OZ"), 10, 0)
model.do()
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [4])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [8])

assert(model.checkPythonDump())