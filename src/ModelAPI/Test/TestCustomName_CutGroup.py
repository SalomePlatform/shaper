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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(70, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 40)
SketchLine_3 = Sketch_1.addLine(0, 40, 70, 40)
SketchLine_4 = Sketch_1.addLine(70, 40, 70, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 70)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), 40)
SketchCircle_1 = Sketch_1.addCircle(30, 70, 20)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 20)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_3.result(), 30, True)
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchCircle_1.center(), SketchLine_2.startPoint(), 30)
SketchLine_5 = Sketch_1.addLine(12.50000000006628, -45.31088913249455, 30, -15)
SketchLine_6 = Sketch_1.addLine(30, -15, 47.50000000006629, -45.31088913249455)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(47.50000000006629, -45.31088913249455, 12.50000000006628, -45.31088913249455)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_7.endPoint())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_5.result(), SketchLine_7.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_5.result(), SketchLine_6.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_7.result(), 35)
SketchConstraintDistanceHorizontal_2 = Sketch_1.setHorizontalDistance(SketchLine_5.endPoint(), SketchLine_2.startPoint(), 30)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_5.endPoint(), SketchLine_2.startPoint(), 15)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 10, 0)
Extrusion_1.result().setName("box")
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_7r-SketchLine_6r-SketchLine_5r")], model.selection(), 20, 20)
Extrusion_2.result().setName("prism")
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f")], model.selection(), 10, 0)
Extrusion_3.result().setName("cylinder")
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "box/From_Face"))
SketchLine_8 = Sketch_2.addLine(45, -10, 25, -10)
SketchLine_9 = Sketch_2.addLine(25, -10, 25, -140)
SketchLine_10 = Sketch_2.addLine(25, -140, 45, -140)
SketchLine_11 = Sketch_2.addLine(45, -140, 45, -10)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_8.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_9.result())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_10.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_11.result())
SketchConstraintLength_4 = Sketch_2.setLength(SketchLine_10.result(), 20)
SketchConstraintLength_5 = Sketch_2.setLength(SketchLine_9.result(), 130)
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "[box/Generated_Face&Sketch_1/SketchLine_3][box/Generated_Face&Sketch_1/SketchLine_2][box/From_Face]"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchConstraintDistanceVertical_2 = Sketch_2.setVerticalDistance(SketchLine_8.endPoint(), SketchAPI_Point(SketchPoint_2).coordinates(), 30)
SketchConstraintDistanceHorizontal_3 = Sketch_2.setHorizontalDistance(SketchLine_8.endPoint(), SketchAPI_Point(SketchPoint_2).coordinates(), 25)
model.do()
Extrusion_4 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_8r-SketchLine_9f-SketchLine_10f-SketchLine_11f")], model.selection(), 5, 15)
Extrusion_4.result().setName("cut_tool")
Cut_1_objects_1 = [model.selection("SOLID", "box"), model.selection("SOLID", "prism"), model.selection("SOLID", "cylinder")]
Cut_1 = model.addCut(Part_1_doc, Cut_1_objects_1, [model.selection("SOLID", "cut_tool")])
model.do()

Extrusions = [Extrusion_1, Extrusion_2, Extrusion_3]
BooleanResults = Cut_1.results()
for i in range(len(BooleanResults)):
  assert(BooleanResults[i].name() == Extrusions[i].result().name()), "Name of result {} of Boolean CUT '{}' != '{}'".format(i, BooleanResults[i].name(), Extrusions[i].result().name())
  BooleanName = Cut_1.name() + "_" + str(i + 1)
  for sub in range(0, BooleanResults[i].numberOfSubs()):
    refName = BooleanName + '_' + str(sub + 1)
    subResult = BooleanResults[i].subResult(sub)
    assert(subResult.name() == refName), "Name of sub-result {} of result {} of Boolean CUT '{}' != '{}'".format(sub, i, subResult.name(), refName)

model.end()

assert(model.checkPythonDump())
