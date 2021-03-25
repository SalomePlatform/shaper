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
SketchLine_1 = Sketch_1.addLine(15, 10, -10, 10)
SketchLine_2 = Sketch_1.addLine(-10, 10, -10, -10)
SketchLine_3 = Sketch_1.addLine(-10, -10, 20, -10)
SketchLine_4 = Sketch_1.addLine(20, -10, 20, 5)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_1.setName("SketchConstraintCoincidence_2")
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_3")
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_3.setName("SketchConstraintCoincidence_4")
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchArc_1 = Sketch_1.addArc(15, 5, 20, 5, 15, 10, False)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_4.endPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_5")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_6")
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_1.result())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 20)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_3.result(), 30)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 5)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchProjection_1.setName("SketchProjection_3")
SketchProjection_1.result().setName("SketchProjection_3")
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchPoint_1.setName("SketchPoint_3")
SketchPoint_1.result().setName("SketchPoint_3")
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_2.startPoint(), SketchAPI_Point(SketchPoint_1).coordinates(), 10)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_2.endPoint(), SketchAPI_Point(SketchPoint_1).coordinates(), 10)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchArc_1_2f")], model.selection(), 10, 0)
Fillet_1 = model.addFillet(Part_1_doc, [model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3]")], 10)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Fillet_1_1/MF:Fillet&Extrusion_1_1/To_Face"))
SketchLine_5 = Sketch_2.addLine(15, 5, 0, 0)
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "[Fillet_1_1/MF:Fillet&Extrusion_1_1/To_Face][Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_1_2]__cc"), False)
SketchProjection_2.setName("SketchProjection_1")
SketchProjection_2.result().setName("SketchProjection_1")
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchPoint_2.setName("SketchPoint_1")
SketchPoint_2.result().setName("SketchPoint_1")
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_5.startPoint(), SketchPoint_2.result())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_7")
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "[Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected][Fillet_1_1/MF:Fillet&Extrusion_1_1/To_Face]__cc"), False)
SketchProjection_3.setName("SketchProjection_2")
SketchProjection_3.result().setName("SketchProjection_2")
SketchPoint_3 = SketchProjection_3.createdFeature()
SketchPoint_3.setName("SketchPoint_2")
SketchPoint_3.result().setName("SketchPoint_2")
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchPoint_3.result())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_8")
model.end()
# check that the second line-point was created on the point, not arc
assert(len(SketchPoint_3.feature().results()) == 1)
assert(SketchPoint_3.feature().firstResult().shape().isVertex())
