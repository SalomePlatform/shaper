## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

# revolution of 4-edges contour (5th is used as axis of rotation) is checking for naming of edges

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(138.1074168797953, 47.3145780051151, 265.9846547314579, 35.80562659846547)
SketchLine_2 = Sketch_1.addLine(248.0818414322251, 186.7007672634271, 138.1074168797954, 234.0153452685422)
SketchLine_2.setName("SketchLine_3")
SketchLine_2.result().setName("SketchLine_3")
SketchLine_3 = Sketch_1.addLine(138.1074168797954, 234.0153452685422, 138.1074168797953, 47.3145780051151)
SketchLine_3.setName("SketchLine_4")
SketchLine_3.result().setName("SketchLine_4")
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_1.setName("SketchConstraintCoincidence_3")
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_4")
SketchLine_4 = Sketch_1.addLine(248.0818414322251, 186.7007672634271, 215.8394027534853, 111.2328618876045)
SketchLine_4.setName("SketchLine_5")
SketchLine_4.result().setName("SketchLine_5")
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_3.setName("SketchConstraintCoincidence_5")
SketchLine_5 = Sketch_1.addLine(215.8394027534853, 111.2328618876045, 265.9846547314579, 35.80562659846547)
SketchLine_5.setName("SketchLine_6")
SketchLine_5.result().setName("SketchLine_6")
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_6")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_5.endPoint())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_7")
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_6r-SketchLine_5r-SketchLine_3f-SketchLine_4f")], model.selection("EDGE", "Sketch_1/SketchLine_4"), 360, 0)
Group_1_objects = [model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_6]"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_6][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5]"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5]"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3][weak_name_3]"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1][weak_name_3]"), model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_6][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_6])"), model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_6][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5])")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
model.do()
model.end()

# check that resulting group selection is valid
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Group_1.feature()))
assert(Group_1.groupList().size() == 7)
for a in range(7):
  assert(Group_1.groupList().value(a).value().shapeTypeStr() == "EDGE")
  assert(len(Group_1.groupList().value(a).namingName()) > 0)

assert(model.checkPythonDump())
