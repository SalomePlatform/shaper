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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchCircle_1 = Sketch_1.addCircle(-155, 278, 145)
SketchCircle_2 = Sketch_1.addCircle(-150, 79, 100)
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r-SketchCircle_2_2f-SketchCircle_2_2f")], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Group_1_objects = [model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][weak_name_1]"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_2][weak_name_1]"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1][weak_name_1]")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_2_objects = [model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Revolution_1_1/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1]"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Revolution_1_1/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_2]"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_2][Revolution_1_1/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1]")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
model.do()
model.end()

# check that resulting group selection is valid
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
for aGroupIter in [Group_1, Group_2]:
  assert(aFactory.validate(aGroupIter.feature()))
  assert(aGroupIter.groupList().size() == 3)
  for a in range(3):
    assert(aGroupIter.groupList().value(a).value().shapeTypeStr() == "EDGE")

assert(model.checkPythonDump())
