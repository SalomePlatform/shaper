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

# Test of deep nested results history. Shell operation.
# Check the initial groups moved to the end contain the corresponding results.

from salome.shaper import model
from ModelAPI import *

aFactory = ModelAPI_Session.get().validators()

def moveGroupsAndVerify(thePart, theLastFeature, theRefNumInGroups):
    # move groups to the end
    model.begin()
    LastFeature = theLastFeature
    for i in range(thePart.size("Groups")):
        GroupFeature = thePart.feature(objectToResult(thePart.object("Groups", 0))) # move always the very first group
        Part_1_doc.moveFeature(GroupFeature, LastFeature)
        LastFeature = GroupFeature
    model.end()

    a = 0
    for i in range(thePart.size("Groups")):
        GroupFeature = thePart.feature(objectToResult(thePart.object("Groups", i)))
        assert(aFactory.validate(GroupFeature))
        assert(GroupFeature.selectionList("group_list").size() == theRefNumInGroups[a])
        a = a + 1


model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(37.25033050129738, -29.31988879634488, -32.3129114537045, -29.31988879634488)
SketchLine_2 = Sketch_1.addLine(-32.3129114537045, -29.31988879634488, -32.3129114537045, 42.23870159689701)
SketchLine_3 = Sketch_1.addLine(-32.3129114537045, 42.23870159689701, 37.25033050129738, 42.23870159689701)
SketchLine_4 = Sketch_1.addLine(37.25033050129738, 42.23870159689701, 37.25033050129738, -29.31988879634488)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Edge_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_3")]
Edge_1 = model.addEdge(Part_1_doc, Edge_1_objects)
Group_1_objects = [model.selection("EDGE", "Edge_1_1"), model.selection("EDGE", "Edge_1_3"), model.selection("EDGE", "Edge_1_4"), model.selection("EDGE", "Edge_1_2")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_2_objects = [model.selection("VERTEX", "Edge_1_1/Modified_Vertex&Sketch_1/SketchLine_1_StartVertex"), model.selection("VERTEX", "Edge_1_2/Modified_Vertex&Sketch_1/SketchLine_4_EndVertex"), model.selection("VERTEX", "Edge_1_1/Modified_Vertex&Sketch_1/SketchLine_1_EndVertex"), model.selection("VERTEX", "Edge_1_3/Modified_Vertex&Sketch_1/SketchLine_2_StartVertex"), model.selection("VERTEX", "Edge_1_4/Modified_Vertex&Sketch_1/SketchLine_3_EndVertex"), model.selection("VERTEX", "Edge_1_2/Modified_Vertex&Sketch_1/SketchLine_4_StartVertex"), model.selection("VERTEX", "Edge_1_3/Modified_Vertex&Sketch_1/SketchLine_2_EndVertex"), model.selection("VERTEX", "Edge_1_4/Modified_Vertex&Sketch_1/SketchLine_3_StartVertex")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Face_1_objects = [model.selection("EDGE", "Edge_1_1"), model.selection("EDGE", "Edge_1_2"), model.selection("EDGE", "Edge_1_3"), model.selection("EDGE", "Edge_1_4")]
Face_1 = model.addFace(Part_1_doc, Face_1_objects)
model.do()
model.end()

num_in_groups = [4, 4]
moveGroupsAndVerify(Part_1_doc, Face_1.feature(), num_in_groups)

assert(model.checkPythonDump())
