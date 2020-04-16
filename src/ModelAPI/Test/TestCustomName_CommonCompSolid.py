# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
Box_1 = model.addBox(Part_1_doc, 100, 100, 10)
Box_1.result().setName("plate")
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "plate/Front"), 50, True)
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "plate/Left"), 50, True)
Partition_1_objects = [model.selection("SOLID", "plate"), model.selection("FACE", "Plane_1"), model.selection("FACE", "Plane_2")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
Partition_1.result().subResult(0).setName("Partition_1_1_1")
Partition_1.result().subResult(1).setName("top_left")
Partition_1.result().subResult(2).setName("bottom_right")
Partition_1.result().subResult(3).setName("Partition_1_1_4")
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Partition_1_1_1/Modified_Face&_plate/Top"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "[Partition_1_1_1/Modified_Face&_plate/Top][Partition_1_1_1/Modified_Face&Plane_2/Plane_2][Partition_1_1_1/Modified_Face&Plane_1/Plane_1]"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(50, 50, 40)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 40)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), model.selection(), 10, model.selection("FACE", "Partition_1_1_1/Modified_Face&_plate/Bottom"), 10)
Extrusion_1.result().setName("cylinder")
Common_1 = model.addCommon(Part_1_doc, [model.selection("COMPSOLID", "plate")], [model.selection("SOLID", "cylinder")])
model.do()

# check the name of result is kept
CommonResult = Common_1.result()
CommonResultName = CommonResult.name()
assert(CommonResultName == Partition_1.result().name()), "Name of Boolean COMMON result '{}' != '{}'".format(CommonResultName, Partition_1.result().name())
# check sub-result names are lost
BooleanName = Common_1.name() + "_1"
for i in range(0, CommonResult.numberOfSubs()):
  refName = BooleanName + '_' + str(i + 1)
  subResult = CommonResult.subResult(i)
  assert(subResult.name() == refName), "Boolean COMMON sub-result {} name '{}' != '{}'".format(i, subResult.name(), refName)

model.end()

assert(model.checkPythonDump())
