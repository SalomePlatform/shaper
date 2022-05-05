# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

# revolution of 3-edges contour (4th is used as axis of rotation) is checking for naming of edges

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(138, 47, 265, 35)
SketchLine_2 = Sketch_1.addLine(265, 35, 248, 186)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(248, 186, 138, 234)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(138, 234, 138, 47)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection("EDGE", "Sketch_1/SketchLine_4"), 360, 0)
Group_1_objects = [model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_2]"), model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_2])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3])2"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3]"), model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_2])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3])"), model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_2])2")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
model.do()
model.end()

# check that resulting group selection is valid
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Group_1.feature()))
assert(Group_1.groupList().size() == 5)
for a in range(5):
  assert(Group_1.groupList().value(a).value().shapeTypeStr() == "EDGE")
  assert(len(Group_1.groupList().value(a).namingName()) > 0)

assert(model.checkPythonDump())
