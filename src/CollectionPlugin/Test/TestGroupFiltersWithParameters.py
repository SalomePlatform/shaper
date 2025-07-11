# Copyright (C) 2014-2025  CEA, EDF
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

# Tests the automatic update of a group of faces when modifying a parameter

from salome.shaper import model
from SketchAPI import *
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "r_in", "0.5")
model.addParameter(Part_1_doc, "r_out", "1")
model.addParameter(Part_1_doc, "l_ext", "2.0")
model.addParameter(Part_1_doc, "r_obst", "0.05")
model.addParameter(Part_1_doc, "l_obst", "0.75")
param_n = model.addParameter(Part_1_doc, "n", "6")

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(
    model.selection("VERTEX", "PartSet/Origin"), False
)
SketchPoint_1 = SketchProjection_1.createdFeature()

### Create SketchCircle
SketchCircle_1 = Sketch_1.addCircle(0, 0, 1)
Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center(), True)

### Create SketchCircle
SketchCircle_2 = Sketch_1.addCircle(0, 0, 0.5)
Sketch_1.setCoincident(
    SketchAPI_Point(SketchPoint_1).coordinates(), SketchCircle_2.center(), True
)
Sketch_1.setRadius(SketchCircle_2.results()[1], "r_in", True)
Sketch_1.setRadius(SketchCircle_1.results()[1], "r_out", True)

### Create SketchCircle
SketchCircle_3 = Sketch_1.addCircle(-0.75, 0, 0.05)
Sketch_1.setRadius(SketchCircle_3.results()[1], "r_obst", True)

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(-0.75, 0, 0, 0)
SketchLine_1.setAuxiliary(True)
Sketch_1.setCoincident(SketchCircle_3.center(), SketchLine_1.startPoint(), True)
Sketch_1.setCoincident(
    SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_1.endPoint(), True
)
Sketch_1.setHorizontal(SketchLine_1.result(), True)
Sketch_1.setLength(SketchLine_1.result(), "l_obst", True)

### Create SketchMultiRotation
SketchMultiRotation_1 = Sketch_1.addRotation(
    [SketchCircle_3.results()[1]], SketchCircle_1.center(), 360, "n", True
)
[SketchCircle_4, SketchCircle_5, SketchCircle_6, SketchCircle_7, SketchCircle_8] = (
    SketchMultiRotation_1.rotatedList()
)

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(
    -0.9238795325112867, 0.3826834323650902, -0.4619397662556438, 0.1913417161825453
)
Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchCircle_1.results()[1], True)
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchCircle_2.results()[1], True)

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(-0.75, 0, -0.5303300858899103, 0.530330085889911)
SketchLine_3.setAuxiliary(True)
Sketch_1.setPerpendicular(SketchLine_3.result(), SketchLine_2.result(), True)

### Create SketchPoint
SketchPoint_2 = Sketch_1.addPoint(-0.6401650429449551, 0.2651650429449555)
Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_3.result(), True)
Sketch_1.setMiddlePoint(SketchPoint_2.coordinates(), SketchLine_3.result(), True)
Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_2.result(), True)
Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchCircle_3.center(), True)

### Create SketchMultiRotation
SketchProjection_2 = Sketch_1.addProjection(
    model.selection("VERTEX", "PartSet/Origin"), False
)
### Create SketchMultiRotation
SketchPoint_3 = SketchProjection_2.createdFeature()

### Create SketchMultiRotation
SketchMultiRotation_2 = Sketch_1.addRotation(
    [SketchLine_2.result()],
    SketchAPI_Point(SketchPoint_3).coordinates(),
    360,
    "n",
    True,
)
[SketchLine_4, SketchLine_5, SketchLine_6, SketchLine_7, SketchLine_8] = (
    SketchMultiRotation_2.rotatedList()
)
model.do()
Sketch_1.changeFacesOrder(
    [
        [
            SketchCircle_2.results()[1],
            SketchCircle_2.results()[1],
            SketchCircle_2.results()[1],
            SketchCircle_2.results()[1],
            SketchCircle_2.results()[1],
            SketchCircle_2.results()[1],
            SketchCircle_2.results()[1],
        ],
        [SketchCircle_8.results()[1]],
        [SketchCircle_3.results()[1]],
        [
            SketchCircle_1.results()[1],
            SketchLine_4.result(),
            SketchCircle_2.results()[1],
            SketchLine_2.result(),
            SketchCircle_3.results()[1],
        ],
        [
            SketchCircle_1.results()[1],
            SketchLine_5.result(),
            SketchCircle_2.results()[1],
            SketchLine_4.result(),
            SketchCircle_4.results()[1],
        ],
        [
            SketchCircle_1.results()[1],
            SketchLine_6.result(),
            SketchCircle_2.results()[1],
            SketchLine_5.result(),
            SketchCircle_5.results()[1],
        ],
        [
            SketchCircle_1.results()[1],
            SketchLine_2.result(),
            SketchCircle_2.results()[1],
            SketchLine_8.result(),
            SketchCircle_8.results()[1],
        ],
        [SketchCircle_7.results()[1]],
        [
            SketchCircle_1.results()[1],
            SketchCircle_1.results()[1],
            SketchLine_7.result(),
            SketchCircle_2.results()[1],
            SketchCircle_2.results()[1],
            SketchLine_6.result(),
            SketchCircle_6.results()[1],
        ],
        [SketchCircle_4.results()[1]],
        [SketchCircle_5.results()[1]],
        [SketchCircle_6.results()[1]],
        [
            SketchCircle_1.results()[1],
            SketchLine_8.result(),
            SketchCircle_2.results()[1],
            SketchLine_7.result(),
            SketchCircle_7.results()[1],
        ],
    ]
)
model.do()

### Create Extrusion
Extrusion_1 = model.addExtrusion(
    Part_1_doc,
    [model.selection("COMPOUND", "Sketch_1")],
    model.selection(),
    "l_ext",
    0,
    "Faces|Wires",
)

### Create Plane
Plane_4 = model.addPlane(
    Part_1_doc, model.selection("FACE", "PartSet/XOY"), "l_ext", False
)

### Create Group
Group_1_objects = [
    model.selection("FACE", "Extrusion_1_1_1/To_Face"),
    model.selection("FACE", "Extrusion_1_1_2/To_Face"),
    model.selection("FACE", "Extrusion_1_1_3/To_Face"),
    model.selection("FACE", "Extrusion_1_1_4/To_Face"),
    model.selection("FACE", "Extrusion_1_1_5/To_Face"),
    model.selection("FACE", "Extrusion_1_1_6/To_Face"),
    model.selection("FACE", "Extrusion_1_1_7/To_Face"),
    model.selection("FACE", "Extrusion_1_1_8/To_Face"),
    model.selection("FACE", "Extrusion_1_1_9/To_Face"),
    model.selection("FACE", "Extrusion_1_1_10/To_Face"),
    model.selection("FACE", "Extrusion_1_1_11/To_Face"),
    model.selection("FACE", "Extrusion_1_1_12/To_Face"),
    model.selection("FACE", "Extrusion_1_1_13/To_Face"),
    model.filters(
        Part_1_doc,
        [
            model.addFilter(
                name="OnPlane",
                args=[
                    model.selection("FACE", "Plane_1"),
                ],
            )
        ],
    ),
]
Group_1 = model.addGroup(Part_1_doc, "Faces", Group_1_objects)
Group_1.result().setColor(0, 0, 255)

model.end()

aFactory = ModelAPI_Session.get().validators()

# Parameter "n" is set to 6 : 13 elems expected in the group
GroupFeature = Group_1.feature()
assert aFactory.validate(GroupFeature)
assert GroupFeature.selectionList("group_list").size() == 13
assert GroupFeature.selectionList("group_list").filters().filters().size() == 1
assert GroupFeature.selectionList("group_list").filters().filters()[0] == 'OnPlane'

# Setting the parameter "n" to 10 : 21 elems expected in the group
model.do()
param_n.setValue(10)
model.end()
assert aFactory.validate(GroupFeature)
assert GroupFeature.selectionList("group_list").size() == 21
assert GroupFeature.selectionList("group_list").filters().filters().size() == 1
assert GroupFeature.selectionList("group_list").filters().filters()[0] == 'OnPlane'

# Setting the parameter "n" to 4 : 9 elems expected in the group
model.do()
param_n.setValue(4)
model.end()
assert aFactory.validate(GroupFeature)
assert GroupFeature.selectionList("group_list").size() == 9
assert GroupFeature.selectionList("group_list").filters().filters().size() == 1
assert GroupFeature.selectionList("group_list").filters().filters()[0] == 'OnPlane'

