# Copyright (C) 2020  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "PartSet/XOY"))
SketchLine_1 = Sketch_1.addLine(0.0004311969126985509, -0.007674652873749346, 0.002931492667495096, -0.004255130738512952)
SketchLine_2 = Sketch_1.addLine(0.002931492667495096, -0.004255130738512952, 0.008777772447092769, -0.005450125033084811)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchArc_1 = Sketch_1.addArc(0.008423929073579738, -0.007181235691194688, 0.008777772447092769, -0.005450125033084811, 0.01017499654536136, -0.007417269487226177, True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchArc_1.startPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_2.result(), SketchArc_1.results()[1])
SketchArc_2 = Sketch_1.addArc(0.008003525163087695, -0.007124567668711833, 0.01017499654536136, -0.007417269487226177, 0.007858546066652927, -0.00931087583093235, True)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchArc_2.startPoint())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchArc_2.results()[1])
SketchArc_3 = Sketch_1.addArc(0.007693833531605691, -0.01179476743082126, 0.007858546066652927, -0.00931087583093235, 0.005279452926115063, -0.01118845563724384, False)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchArc_3.startPoint())
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchArc_3.results()[1])
SketchEllipticArc_1 = Sketch_1.addEllipticArc(0.003574951141940773, -0.01155552979705393, 0.002239383243347156, -0.01179560634192411, 0.005279452926115063, -0.01118845563724384, 0.002020289862486981, -0.01123808958696058, False)
[SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchLine_3, SketchLine_4] = SketchEllipticArc_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchEllipticArc_1.startPoint())
SketchLine_5 = Sketch_1.addLine(0.002020289862504265, -0.0112380895869727, 0.0004311969126985505, -0.007674652873749346)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchEllipticArc_1.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.endPoint())
SketchCircle_1 = Sketch_1.addCircle(0.003364447146935956, -0.00551738944900832, 0.001819824088768701)
SketchCircle_2 = Sketch_1.addCircle(0.007829307057027779, -0.01091774381645273, 0.003225214826443006)
model.do()
Sketch_1.changeFacesOrder([[SketchLine_5.result(), SketchEllipticArc_1.result(), SketchCircle_2.results()[1], SketchArc_2.results()[1], SketchArc_1.results()[1], SketchLine_2.result(), SketchCircle_1.results()[1], SketchLine_1.result()],
                           [SketchCircle_1.results()[1], SketchLine_2.result(), SketchLine_1.result()],
                           [SketchLine_1.result(), SketchLine_2.result(), SketchCircle_1.results()[1], SketchCircle_1.results()[1]],
                           [SketchArc_2.results()[1], SketchCircle_2.results()[1], SketchEllipticArc_1.result(), SketchArc_3.results()[1]],
                           [SketchArc_3.results()[1], SketchEllipticArc_1.result(), SketchCircle_2.results()[1], SketchCircle_2.results()[1], SketchArc_2.results()[1]]
                          ])
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")])
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], True)
Edge_1 = model.addEdge(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], True)
Wire_1 = model.addWire(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], True)

model.end()

assert(len(Face_1.results()) == 5)
assert(len(Vertex_1.results()) == 25)
assert(len(Edge_1.results()) == 17)
assert(len(Wire_1.results()) == 3)

assert(model.checkPythonDump())
