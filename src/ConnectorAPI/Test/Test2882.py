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

import salome
salome.standalone()
salome.salome_init(1)

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-20, -11.54700538379252, -40, 11.54700538379252)
SketchLine_2 = Sketch_1.addLine(-40, 11.54700538379252, -20, 11.54700538379251)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-20, 11.54700538379251, -40, -11.54700538379252)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-40, -11.54700538379252, -20, -11.54700538379252)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
SketchLine_5 = Sketch_1.addLine(-20, 11.54700538379251, -20, -11.54700538379252)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.endPoint())
SketchLine_6 = Sketch_1.addLine(-15, 8.660254037844391, -15, -8.660254037844384)
SketchLine_7 = Sketch_1.addLine(-15, -8.660254037844384, 0, 0)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(0, 0, -15, 8.660254037844391)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchLine_8.endPoint())
SketchCircle_1 = Sketch_1.addCircle(12, 0, 10)
SketchLine_9 = Sketch_1.addLine(7, 8.660254037844386, 17, 8.660254037844386)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_9.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchCircle_1.results()[1])
SketchLine_10 = Sketch_1.addLine(7, -8.660254037844386, 17, -8.660254037844386)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_10.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_8.result(), SketchLine_3.startPoint())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_7.result())
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_9.result(), SketchLine_6.startPoint())
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_10.result(), SketchLine_6.endPoint())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_2.result(), SketchLine_4.result())
SketchConstraintParallel_2 = Sketch_1.setParallel(SketchLine_10.result(), SketchLine_9.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_2.result(), SketchLine_4.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_7.result(), SketchLine_8.result())
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_10.result(), SketchLine_9.result())
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_1.result())
SketchConstraintParallel_3 = Sketch_1.setParallel(SketchLine_6.result(), SketchLine_5.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 10)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_9.result(), 10)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_7.endPoint(), 12, True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_7.endPoint(), SketchLine_5.result(), 20, True)
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_9.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchConstraintEqual_5 = Sketch_1.setEqual(SketchLine_6.result(), SketchLine_8.result())
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), 20)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), 10, 0)
Group_1_objects = [model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_4"), model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchLine_5"), model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_2"), model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_1"), model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_3"), model.selection("FACE", "Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_6"), model.selection("FACE", "Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_7"), model.selection("FACE", "Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_8")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_1.setName("planar")
Group_1.result().setName("planar")
Group_2 = model.addGroup(Part_1_doc, [model.selection("FACE", "Extrusion_1_2/From_Face"), model.selection("FACE", "Extrusion_1_2/To_Face")])
Group_2.setName("triangle_top_bottom")
Group_2.result().setName("triangle_top_bottom")
model.do()
model.exportToGEOM(Part_1_doc)
model.end()

# check what is exported to GEOM
import salome
from salome.geom import geomBuilder

import os
import tempfile

aComp = salome.myStudy.FindComponent("GEOM")
iterator = salome.myStudy.NewChildIterator(aComp)
aComponents = []
while iterator.More():
  aComponents.append(iterator.Value())
  iterator.Next()

assert len(aComponents) == 3

ok, group_0_1 = aComponents[0].FindSubObject(1)
assert ok
assert group_0_1.GetName() == "planar"

ok, group_0_2 = aComponents[0].FindSubObject(2)
assert not ok


ok, group_1_1 = aComponents[1].FindSubObject(1)
assert ok
assert group_1_1.GetName() == "planar"

ok, group_1_2 = aComponents[1].FindSubObject(2)
assert ok
assert group_1_2.GetName() == "triangle_top_bottom"

ok, group_1_3 = aComponents[1].FindSubObject(3)
assert not ok


ok, group_2_1 = aComponents[2].FindSubObject(1)
assert not ok
