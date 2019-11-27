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

# Test move the group in history for selection of a whole features
# with many modifications of this feature, and joining them in one.

from SketchAPI import *

from salome.shaper import model
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-20, 9.999999999999998, -40, 10)
SketchLine_2 = Sketch_1.addLine(-40, 10, -40, -10)
SketchLine_3 = Sketch_1.addLine(-40, -10, -20, -9.999999999999998)
SketchLine_4 = Sketch_1.addLine(-20, -9.999999999999998, -20, 9.999999999999998)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 10)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_1.result(), SketchCircle_1.results()[1])
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchLine_3.result(), SketchCircle_1.results()[1])
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_4.result(), 20, True)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 10)
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 30, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f")], model.selection(), 15, 0)
Group_1 = model.addGroup(Part_1_doc, "Vertices", [model.selection("COMPOUND", "all-in-Extrusion_1")])
Group_2 = model.addGroup(Part_1_doc, "Vertices", [model.selection("COMPOUND", "all-in-Extrusion_2")])
Group_3 = model.addGroup(Part_1_doc, "Faces", [model.selection("COMPOUND", "all-in-Extrusion_1"), model.selection("COMPOUND", "all-in-Extrusion_2")])
ExtrusionFuse_1 = model.addExtrusionFuse(Part_1_doc, [], model.selection(), 12, 0, [model.selection("SOLID", "Extrusion_1_1")])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4"))
SketchCircle_2 = Sketch_2.addCircle(-2.3396523840492e-15, 15, 7)
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_2.results()[1], 7)
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4]"), False)
SketchLine_5 = SketchProjection_2.createdFeature()
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchCircle_2.center(), SketchLine_5.result(), 10, True)
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1/To_Face]"), False)
SketchLine_6 = SketchProjection_3.createdFeature()
SketchConstraintDistance_3 = Sketch_2.setDistance(SketchCircle_2.center(), SketchLine_6.result(), 15, True)
ExtrusionFuse_1.setNestedSketch(Sketch_2)
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 50, 50, [model.selection("SOLID", "ExtrusionFuse_1_1"), model.selection("SOLID", "Extrusion_2_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "ExtrusionFuse_1_1/To_Face"))
SketchProjection_4 = Sketch_3.addProjection(model.selection("VERTEX", "[ExtrusionFuse_1_1/Generated_Face&Sketch_2/SketchCircle_2_2][ExtrusionFuse_1_1/To_Face]__cc"), False)
SketchPoint_2 = SketchProjection_4.createdFeature()
SketchCircle_3 = Sketch_3.addCircle(-2.3396523840492e-15, 15, 5)
SketchConstraintCoincidence_6 = Sketch_3.setCoincident(SketchPoint_2.result(), SketchCircle_3.center())
SketchConstraintRadius_3 = Sketch_3.setRadius(SketchCircle_3.results()[1], 5)
ExtrusionCut_1.setNestedSketch(Sketch_3)
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "ExtrusionCut_1_1"), model.selection("SOLID", "ExtrusionCut_1_2")], True, keepSubResults = True)
model.do()
# move groups after the final fuse
Part_1_doc.moveFeature(Group_1.feature(), Fuse_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_1_doc.moveFeature(Group_3.feature(), Group_2.feature())
model.end()

aFactory = ModelAPI_Session.get().validators()
for group in [Group_1, Group_2, Group_3]:
  selectionList = group.feature().selectionList("group_list")
  assert(selectionList.size() == 1)
  assert(aFactory.validate(group.feature()))
