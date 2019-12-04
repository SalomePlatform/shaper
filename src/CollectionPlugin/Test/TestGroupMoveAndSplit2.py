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

# Check the movement to the end and split: move to intermediate position, no duplicates appeared

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 10)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 10)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f")], model.selection(), 10, 0)
Group_1 = model.addGroup(Part_1_doc, "Edges", [model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/To_Face]")])
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 0, 5, [model.selection("SOLID", "Extrusion_1_1")])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/To_Face]"), False)
SketchCircle_2 = SketchProjection_2.createdFeature()
SketchCircle_3 = Sketch_2.addCircle(0, 10, 3)
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchCircle_2.results()[1], SketchCircle_3.center())
SketchCircle_4 = Sketch_2.addCircle(0, -10, 3)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchCircle_2.results()[1], SketchCircle_4.center())
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OY"), True)
SketchLine_1 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchCircle_3.center(), SketchLine_1.result())
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchCircle_4.center(), SketchLine_1.result())
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_3.results()[1], 3)
SketchConstraintEqual_1 = Sketch_2.setEqual(SketchCircle_3.results()[1], SketchCircle_4.results()[1])
ExtrusionCut_1.setNestedSketch(Sketch_2)

ExtrusionCut_2 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 0, 3, [model.selection("SOLID", "ExtrusionCut_1_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/To_Face"))
SketchLine_2 = Sketch_3.addLine(10, 2, -10, 2)
SketchLine_3 = Sketch_3.addLine(-10, 2, -10, -2)
SketchLine_4 = Sketch_3.addLine(-10, -2, 10, -2)
SketchLine_5 = Sketch_3.addLine(10, -2, 10, 2)
SketchConstraintCoincidence_6 = Sketch_3.setCoincident(SketchLine_5.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_7 = Sketch_3.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_8 = Sketch_3.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_9 = Sketch_3.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintHorizontal_1 = Sketch_3.setHorizontal(SketchLine_2.result())
SketchConstraintVertical_1 = Sketch_3.setVertical(SketchLine_3.result())
SketchConstraintHorizontal_2 = Sketch_3.setHorizontal(SketchLine_4.result())
SketchConstraintVertical_2 = Sketch_3.setVertical(SketchLine_5.result())
SketchProjection_4 = Sketch_3.addProjection(model.selection("EDGE", "([ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/From_Face])2(ExtrusionCut_1_1/Generated_Edge&ExtrusionCut_1_1/From_Face_1)2([ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/To_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchCircle_4_2])2"), False)
SketchArc_1 = SketchProjection_4.createdFeature()
SketchConstraintTangent_1 = Sketch_3.setTangent(SketchLine_5.result(), SketchArc_1.results()[1])
SketchProjection_5 = Sketch_3.addProjection(model.selection("EDGE", "([ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/To_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchCircle_4_2])(ExtrusionCut_1_1/Generated_Edge&ExtrusionCut_1_1/From_Face_3)2(ExtrusionCut_1_1/Generated_Edge&ExtrusionCut_1_1/From_Face_2)2([ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchCircle_4_2][ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face_3])2"), False)
SketchArc_2 = SketchProjection_5.createdFeature()
SketchConstraintTangent_2 = Sketch_3.setTangent(SketchArc_2.results()[1], SketchLine_3.result())
SketchConstraintDistanceVertical_1 = Sketch_3.setVerticalDistance(SketchAPI_Arc(SketchArc_1).center(), SketchLine_2.startPoint(), 2)
SketchConstraintDistanceVertical_2 = Sketch_3.setVerticalDistance(SketchAPI_Arc(SketchArc_1).center(), SketchLine_4.endPoint(), 2)
ExtrusionCut_2.setNestedSketch(Sketch_3)
model.do()
# move only after the first extrusion-cut
Part_1_doc.setCurrentFeature(ExtrusionCut_1.feature(), True)
model.do()
Part_1_doc.moveFeature(Group_1.feature(), ExtrusionCut_1.feature(), True)
model.end()
assert(Part_1_doc.size("Groups") == 3) # 3 edges in groups results

# check that simple move to the end provides 4 edges (no duplicates)
model.undo()
model.undo()

model.begin()
Part_1_doc.moveFeature(Group_1.feature(), ExtrusionCut_2.feature(), True)
model.end()
assert(Part_1_doc.size("Groups") == 4) # 4 edges in groups results

assert(model.checkPythonDump())
