# Copyright (C) 2021-2022  CEA/DEN, EDF R&D
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

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 1)

### Create RevolutionFuse
RevolutionFuse_1 = model.addRevolutionFuse(Part_1_doc, [], model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), 80, 0, [model.selection("SOLID", "Box_1_1")])

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))

### Create SketchArc
SketchArc_1 = Sketch_1.addArc(4.7, 4.9, 1.8, 3.2, 7.6, 3.2, True)

### Create SketchArc
SketchArc_2 = Sketch_1.addArc(4.7, 4.9, 1.4, 3.2, 8.1, 3.2, True)

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(1.4, 3.2, 1.8, 3.2)
Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_1.startPoint())
Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_1.endPoint())

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(7.6, 3.2, 8.1, 3.2)
Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_2.startPoint())
Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_2.endPoint())
Sketch_1.setCoincident(SketchArc_1.center(), SketchArc_2.center())
Sketch_1.setHorizontal(SketchLine_1.result())
Sketch_1.setCollinear(SketchLine_1.result(), SketchLine_2.result())
Sketch_1.setRadius(SketchArc_1.results()[1], 3.5)
Sketch_1.setRadius(SketchArc_2.results()[1], 4)
RevolutionFuse_1.setNestedSketch(Sketch_1)

### Create Group
Group_1_objects = [model.selection("FACE", "RevolutionFuse_1_1/Modified_Face&Box_1_1/Top"),
                   model.selection("FACE", "RevolutionFuse_1_1/Generated_Face&Sketch_1/SketchArc_1_2"),
                   model.selection("FACE", "RevolutionFuse_1_1/Generated_Face&Sketch_1/SketchArc_2_2"),
                   model.selection("FACE", "RevolutionFuse_1_1/To_Face")]
Group_1 = model.addGroup(Part_1_doc, "Faces", Group_1_objects)
Group_1.setName("Group_faces")
Group_1.result().setName("Group_faces")

### Create GroupShape
GroupShape_1 = model.addGroupShape(Part_1_doc, [model.selection("COMPOUND", "Group_faces")])
GroupShape_1.result().setName("GroupShape_1")

### Create Shell
Shell_1 = model.addShell(Part_1_doc, [model.selection("COMPOUND", "GroupShape_1")])
Shell_1.result().setName("Shell_1_1")

### Create Group
Group_2_objects = [model.selection("EDGE", "[RevolutionFuse_1_1/To_Face][GroupShape_1_1_3/GroupShape_1_1_3]"),
                   model.selection("EDGE", "[GroupShape_1_1_3/GroupShape_1_1_3][new_weak_name_1]"),
                   model.selection("EDGE", "[GroupShape_1_1_3/GroupShape_1_1_3][new_weak_name_4]")]
Group_2 = model.addGroup(Part_1_doc, "Edges", Group_2_objects)
Group_2.setName("Group_edges")
Group_2.result().setName("Group_edges")

### Create GroupShape
GroupShape_2 = model.addGroupShape(Part_1_doc, [model.selection("COMPOUND", "Group_edges")])
GroupShape_2.result().setName("GroupShape_2")

### Create Wire
Wire_1_objects = [model.selection("EDGE", "GroupShape_2_1_2"),
                  model.selection("EDGE", "GroupShape_2_1_3"),
                  model.selection("EDGE", "GroupShape_2_1_1")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects, False)
Wire_1.result().setName("Wire_1_1")

model.end()

#=============================================================================
# Tests :
#=============================================================================
# Group of edges :
model.checkResult(Wire_1, model, 1, [0], [0], [0], [3], [6])
r=Wire_1.defaultResult()
s=r.shape()
dim=s.computeSize()
dim=dim[1:]
dx=abs(dim[3]-dim[0])
dy=abs(dim[4]-dim[1])
dz=abs(dim[5]-dim[2])
print(dx,dy,dz)
tol=1e-06
assert(abs(dx-7.0000002) <= tol)
assert(abs(dy-2.6261643) <= tol)
assert(abs(dz-8.46802318) <= tol)
