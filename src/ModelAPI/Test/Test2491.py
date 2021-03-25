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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-17.23654191510763, 16.36770059625792, -4.760492650097915, 19.42518236023452)
SketchLine_2 = Sketch_1.addLine(20, 2.492755359431925e-15, 20, -31)
SketchLine_3 = Sketch_1.addLine(-25.74772708486752, -20, -17.21320343559642, -20)
SketchLine_4 = Sketch_1.addLine(-13.67766952966368, -21.46446609406726, 2.928932188134523, -38.07106781186547)
SketchArc_1 = Sketch_1.addArc(-17.21320343559642, -25, -13.67766952966368, -21.46446609406726, -17.21320343559642, -20, False)
Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_4.startPoint())
Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_3.endPoint())
Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_4.result())
Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_3.result())
Sketch_1.setHorizontal(SketchLine_3.result())
Sketch_1.setVertical(SketchLine_2.result())
SketchArc_2 = Sketch_1.addArc(0, 6.520200294596256e-23, -4.760492650097915, 19.42518236023452, 20, 2.492755359431925e-15, True)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchArc_2.startPoint())
SketchArc_3 = Sketch_1.addArc(-12, -5, -29.79117622747561, -17.94117647058824, -17.23654191510763, 16.36770059625792, True)
Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_1.startPoint())
Sketch_1.setTangent(SketchArc_3.results()[1], SketchLine_1.result())
Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_2.startPoint())
SketchArc_4 = Sketch_1.addArc(-25.74772708486752, -15, -29.79117622747561, -17.94117647058824, -25.74772708486752, -20, False)
Sketch_1.setCoincident(SketchArc_4.startPoint(), SketchArc_3.startPoint())
Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchLine_3.startPoint())
Sketch_1.setTangent(SketchArc_4.results()[1], SketchLine_3.result())
Sketch_1.setTangent(SketchArc_4.results()[1], SketchArc_3.results()[1])
SketchArc_5 = Sketch_1.addArc(10, -31, 20, -31, 2.928932188134523, -38.07106781186547, True)
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchArc_5.startPoint())
Sketch_1.setCoincident(SketchArc_5.endPoint(), SketchLine_4.endPoint())
Sketch_1.setTangent(SketchArc_5.results()[1], SketchLine_4.result())
Sketch_1.setTangent(SketchLine_2.result(), SketchArc_5.results()[1])
Sketch_1.setTangent(SketchLine_1.result(), SketchArc_2.results()[1])
Sketch_1.setLength(SketchLine_2.result(), 31)
Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_2.result())
Sketch_1.setAngle(SketchLine_3.result(), SketchLine_4.result(), 135)
Sketch_1.setRadius(SketchArc_1.results()[1], 5)
Sketch_1.setEqual(SketchArc_4.results()[1], SketchArc_1.results()[1])
Sketch_1.setRadius(SketchArc_5.results()[1], 10)
Sketch_1.setRadius(SketchArc_2.results()[1], 20)
Sketch_1.setRadius(SketchArc_3.results()[1], 22)
SketchCircle_1 = Sketch_1.addCircle(-22, -5, 7.5)
SketchLine_5 = Sketch_1.addLine(-22, -5, -12, -5)
SketchLine_5.setAuxiliary(True)
Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_5.startPoint())
Sketch_1.setCoincident(SketchArc_3.center(), SketchLine_5.endPoint())
Sketch_1.setHorizontal(SketchLine_5.result())
Sketch_1.setRadius(SketchCircle_1.results()[1], 7.5)
SketchCircle_2 = Sketch_1.addCircle(0, 6.520200294596256e-23, 10)
Sketch_1.setCoincident(SketchArc_2.center(), SketchCircle_2.center())
Sketch_1.setRadius(SketchCircle_2.results()[1], 10)
Sketch_1.setVerticalDistance(SketchCircle_2.center(), SketchArc_3.center(), 5)
SketchCircle_3 = Sketch_1.addCircle(7, -25, 7.5)
Sketch_1.setRadius(SketchCircle_3.results()[1], 7.5)
Sketch_1.setHorizontalDistance(SketchArc_2.center(), SketchCircle_3.center(), 7)
Sketch_1.setDistance(SketchCircle_2.center(), SketchLine_3.result(), 20, True)
Sketch_1.setVerticalDistance(SketchArc_2.center(), SketchCircle_3.center(), 25)
Sketch_1.setHorizontalDistance(SketchLine_5.endPoint(), SketchArc_2.center(), 12)
Sketch_1.setHorizontalDistance(SketchCircle_1.center(), SketchArc_2.center(), 22)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
Sketch_1.setCoincident(SketchCircle_2.center(), SketchAPI_Point(SketchPoint_1).coordinates())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_3_2f-SketchArc_4_2f-SketchLine_3f-SketchArc_1_2r-SketchLine_4f-SketchArc_5_2f-SketchLine_2r-SketchArc_2_2f-SketchLine_1r-SketchCircle_1_2r-SketchCircle_2_2r-SketchCircle_3_2r")], model.selection(), 6.5, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2"))
SketchLine_6 = Sketch_2.addLine(14.45330132699559, 5.092287733454298, -12.15814035598059, 5.092287733454298)
SketchLine_7 = Sketch_2.addLine(-12.15814035598059, 5.092287733454298, -12.15814035598059, 23.78152371848275)
SketchLine_8 = Sketch_2.addLine(-12.15814035598059, 23.78152371848275, 14.45330132699559, 23.78152371848275)
SketchLine_9 = Sketch_2.addLine(14.45330132699559, 23.78152371848275, 14.45330132699559, 5.092287733454298)
Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_6.startPoint())
Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
Sketch_2.setHorizontal(SketchLine_6.result())
Sketch_2.setVertical(SketchLine_7.result())
Sketch_2.setHorizontal(SketchLine_8.result())
Sketch_2.setVertical(SketchLine_9.result())
model.do()
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_9r-SketchLine_8r-SketchLine_7r-SketchLine_6r")], model.selection(), 0, 100, [model.selection("SOLID", "Extrusion_1_1")])
model.do()
model.testHaveNamingSubshapes(Extrusion_1, model, Part_1_doc)
model.end()
assert(model.checkPythonDump())
