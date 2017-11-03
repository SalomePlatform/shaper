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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "g", "5")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 200, 0, 0)
SketchLine_2 = Sketch_1.addLine(0, 0, 250, 0)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(500, 0, 500, 200)
SketchArc_1 = Sketch_1.addArc(250, 200.000000000003, 0, 200, 500, 200, True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_3")
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_1.result(), SketchArc_1.results()[1])
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.result(), SketchArc_1.endPoint())
SketchConstraintCoincidence_3.setName("SketchConstraintCoincidence_4")
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_3.endPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_5")
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 250)
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchPoint_1.coordinates())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_6")
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), 200)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_1.result(), 200)
SketchArc_2 = Sketch_1.addArc(250, 0, 1.232595164407831e-032, 0, 500, 0, False)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchArc_2.center())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_7")
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_8")
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_2.endPoint())
SketchConstraintCoincidence_8.setName("SketchConstraintCoincidence_9")
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_2.endPoint())
SketchConstraintCoincidence_9.setName("SketchConstraintCoincidence_10")
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchArc_2.results()[1], SketchArc_2.startPoint())
SketchConstraintCoincidence_10.setName("SketchConstraintCoincidence_11")
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_11.setName("SketchConstraintCoincidence_12")
model.do()
Sketch_1.setName("toto")
Sketch_1.result().setName("tutu")
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "tutu/Face-SketchLine_1f-SketchLine_3f-SketchArc_1_2f-SketchArc_2_2f")], model.selection(), 10, 0)
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "tutu/Edge-SketchLine_2")])
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("EDGE", "Edge_1_1")], model.selection("EDGE", "PartSet/OZ"), 100, 0)
Group_1 = model.addGroup(Part_1_doc, [model.selection("VERTEX", "Edge_1_1/Generated_Edge_2&Edge_1_1/To_Edge_1"), model.selection("VERTEX", "Edge_1_1/Generated_Edge_1&Edge_1_1/To_Edge_1")])
model.end()

# check that resulting group selection is valid
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Group_1.feature()))
assert(Group_1.groupList().size() == 2)
assert(Group_1.groupList().value(0).value().shapeTypeStr() == "VERTEX")
assert(Group_1.groupList().value(1).value().shapeTypeStr() == "VERTEX")

assert(model.checkPythonDump())
