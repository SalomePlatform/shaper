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

# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(10.96877310597264, 4.388320842434023, 10.96877310597264, 75.63259202833271)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_2 = Sketch_2.addLine(20.69511709668412, -6.720507935503198, 19.39808085030927, 19.46570269579372)
SketchLine_3 = Sketch_2.addLine(19.39808085030927, 19.46570269579372, 61.20243103906213, 28.02128023393725)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_2.addLine(61.20243103906213, 28.02128023393725, 64.54729867171925, -32.684941224703)
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_2.addLine(64.54729867171925, -32.684941224703, 20.69511709668412, -6.720507935503198)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_2.startPoint(), SketchLine_5.endPoint())
model.do()
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex"))
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchLine_6 = Sketch_3.addLine(18.78449167586506, 11.40320166178118, 18.60732592394469, -15.47067574876233)
SketchLine_7 = Sketch_3.addLine(18.60732592394469, -15.47067574876233, 57.06415928764967, -9.455161348995592)
SketchConstraintCoincidence_5 = Sketch_3.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_3.addLine(57.06415928764967, -9.455161348995592, 56.5687276408287, 11.2362868340608)
SketchConstraintCoincidence_6 = Sketch_3.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_3.addLine(56.5687276408287, 11.2362868340608, 18.78449167586506, 11.40320166178118)
SketchConstraintCoincidence_7 = Sketch_3.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_8 = Sketch_3.setCoincident(SketchLine_6.startPoint(), SketchLine_9.endPoint())
model.do()
Pipe_1 = model.addPipe(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_5r-SketchLine_4r-SketchLine_3r-SketchLine_2r"), model.selection("FACE", "Sketch_3/Face-SketchLine_6r-SketchLine_7f-SketchLine_8f-SketchLine_9f")], model.selection("EDGE", "Sketch_1/SketchLine_1"), [model.selection("VERTEX", "Sketch_1/SketchLine_1_StartVertex"), model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex")])
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Pipe_1, 1)
model.testNbSubResults(Pipe_1, [0])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.VERTEX, [48])

assert(model.checkPythonDump())
