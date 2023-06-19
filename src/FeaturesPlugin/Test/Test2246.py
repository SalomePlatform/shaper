# Copyright (C) 2014-2023  CEA, EDF
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

# check that revolution that produces a sphere has correctly named 2 vertices with different names

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchLine_1 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OZ"))
SketchArc_1 = Sketch_1.addArc(0, 0, 0, 62, 0, -62, False)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchArc_1.center())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.result())
SketchLine_2 = Sketch_1.addLine(0, -62, 0, 62)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_2.endPoint())
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_1_2f-SketchLine_2f")], model.selection("EDGE", "Sketch_1/SketchLine_2"), 360, 0)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Revolution_1_1")], model.selection("EDGE", "PartSet/OY"), 150)
Group_1 = model.addGroup(Part_1_doc, [model.selection("VERTEX", "[Translation_1_1/MF:Translated&Sketch_1/SketchArc_1_2][weak_name_1]"), model.selection("VERTEX", "[Translation_1_1/MF:Translated&Sketch_1/SketchArc_1_2][weak_name_2]")])
model.do()
model.end()

# check that resulting group selection is valid, names exist and different
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Group_1.feature()))
assert(Group_1.groupList().size() == 2)
assert(Group_1.groupList().value(0).value().shapeTypeStr() == "VERTEX")
assert(len(Group_1.groupList().value(0).namingName()) > 0)
assert(Group_1.groupList().value(1).value().shapeTypeStr() == "VERTEX")
assert(Group_1.groupList().value(0).namingName() != Group_1.groupList().value(1).namingName())

assert(model.checkPythonDump())
