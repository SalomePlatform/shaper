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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(80, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 80)
SketchLine_3 = Sketch_1.addLine(0, 80, 80, 80)
SketchLine_4 = Sketch_1.addLine(80, 80, 80, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(35, 80, 65, 0)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_3.result())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_1.result())
SketchLine_6 = Sketch_1.addLine(0, 20, 80, 50)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchLine_2.result())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_4.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_5.result(), SketchLine_6.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 80)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_6.startPoint(), SketchLine_1.endPoint(), 20)
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_5.endPoint(), SketchLine_1.startPoint(), 15)
SketchConstraintDistanceVertical_2 = Sketch_1.setVerticalDistance(SketchLine_6.endPoint(), SketchLine_1.startPoint(), 50)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 50, 0)
Extrusion_1.result().setName("compsolid")
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1_4/Generated_Face&Sketch_1/SketchLine_5"))
SketchCircle_1 = Sketch_2.addCircle(65, -30, 10)
SketchConstraintRadius_1 = Sketch_2.setRadius(SketchCircle_1.results()[1], 10)
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1_3/Generated_Face&Sketch_1/SketchLine_5][Extrusion_1_1_3/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1_3/To_Face]"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchConstraintDistanceVertical_3 = Sketch_2.setVerticalDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchCircle_1.center(), 20)
SketchConstraintDistanceHorizontal_2 = Sketch_2.setHorizontalDistance(SketchCircle_1.center(), SketchAPI_Point(SketchPoint_2).coordinates(), 15)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2r")], model.selection(), model.selection(), 10, model.selection("FACE", "Extrusion_1_1_4/Generated_Face&Sketch_1/SketchLine_3"), 10)
Extrusion_2.result().setName("cyl")
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1_3")], [model.selection("SOLID", "cyl")])

model.do()

# check the name of Extrusion_1 is kept
CutResult = Cut_1.result()
CutResultName = CutResult.name()
assert(CutResultName == Extrusion_1.result().name()), "Name of Boolean CUT result '{}' != '{}'".format(CutResultName, Extrusion_1.result().name())
# check sub-result names are lost
BooleanName = Cut_1.name() + "_1"
for i in range(0, CutResult.numberOfSubs()):
  refName = BooleanName + '_' + str(i + 1)
  subResult = CutResult.subResult(i)
  assert(subResult.name() == refName), "Boolean CUT sub-result {} name '{}' != '{}'".format(i, subResult.name(), refName)

model.end()

assert(model.checkPythonDump())
