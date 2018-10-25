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

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Point_2 = model.addPoint(Part_1_doc, 5, 20, 0)
Point_3 = model.addPoint(Part_1_doc, 0, 40, 30)
Point_4 = model.addPoint(Part_1_doc, 10, 50, 70)
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_1")])
Vertex_2 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "PartSet/Origin")])
Vertex_3 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_2")])
Vertex_4 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_3")])
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))
SketchLine_1 = Sketch_1.addLine(14.65001594578831, -16.75950688440132, 21.53989436538019, 4.084511469683225)
SketchLine_1.setName("SketchLine_2")
SketchLine_1.result().setName("SketchLine_1")
SketchLine_2 = Sketch_1.addLine(21.53989436538019, 4.084511469683225, 17.92440575878417, 20.67042582762886)
SketchLine_2.setName("SketchLine_3")
SketchLine_2.result().setName("SketchLine_3")
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_1.setName("SketchConstraintCoincidence_2")
SketchLine_3 = Sketch_1.addLine(17.92440575878417, 20.67042582762886, 6.041293582700577, 31.26820121140768)
SketchLine_3.setName("SketchLine_4")
SketchLine_3.result().setName("SketchLine_4")
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_3")
SketchArc_1 = Sketch_1.addArc(15.73021805614924, 42.13223372900752, 6.041293582700577, 31.26820121140768, 1.302016307518145, 40.2010542198842, True)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_3.endPoint())
SketchConstraintCoincidence_3.setName("SketchConstraintCoincidence_4")
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_3.result())
SketchArc_2 = Sketch_1.addArc(5.884297127208473, -13.86204556507233, 14.65001594578831, -16.75950688440132, -3.31891814825968, -14.59275556007859, True)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_9")
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_1.result())
SketchConstraintRigid_1 = Sketch_1.setFixed(SketchArc_1.startPoint())
SketchConstraintRigid_2 = Sketch_1.setFixed(SketchLine_2.endPoint())
model.do()
Wire_1_objects = [model.selection("EDGE", "Sketch_1/SketchArc_2_2"), model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchArc_1_2")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchLine_4 = Sketch_2.addLine(9.525599451781, 2.539525405208, -21.231656152164, 2.539525405208)
SketchLine_4.setName("SketchLine_5")
SketchLine_4.result().setName("SketchLine_5")
SketchLine_5 = Sketch_2.addLine(-21.231656152164, 2.539525405208, -21.231656152164, -7.57049842757)
SketchLine_5.setName("SketchLine_6")
SketchLine_5.result().setName("SketchLine_6")
SketchLine_6 = Sketch_2.addLine(-21.231656152164, -7.57049842757, 9.525599451781, -7.57049842757)
SketchLine_6.setName("SketchLine_7")
SketchLine_6.result().setName("SketchLine_7")
SketchLine_7 = Sketch_2.addLine(9.525599451781, -7.57049842757, 9.525599451781, 2.539525405208)
SketchLine_7.setName("SketchLine_8")
SketchLine_7.result().setName("SketchLine_8")
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_5")
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_6")
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_7")
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8.setName("SketchConstraintCoincidence_8")
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_4.result())
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_5.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_6.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_7.result())
model.do()
Pipe_1 = model.addPipe(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection("WIRE", "Wire_1_1"))
Pipe_2 = model.addPipe(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection("EDGE", "Sketch_1/SketchArc_2_2"))
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Pipe_2_1")], [model.selection("SOLID", "Pipe_1_1")])
model.do()
model.end()

# Check that the small pipe is a part of the big one
assert(len(Cut_1.results()) == 0)
