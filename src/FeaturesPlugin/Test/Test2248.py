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

# check that extrusion of sketch with intersected edges produces well-named edges

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(394.1014919414247, 282.461820162796, -355.0948596332293, 282.461820162796)
SketchLine_2 = Sketch_1.addLine(-355.0948596332293, 282.461820162796, -355.0948596332293, -224.6243998437472)
SketchLine_3 = Sketch_1.addLine(-355.0948596332293, -224.6243998437472, 394.1014919414247, -224.6243998437472)
SketchLine_4 = Sketch_1.addLine(394.1014919414247, -224.6243998437472, 394.1014919414247, 282.461820162796)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchArc_1 = Sketch_1.addArc(-7.174842024158346, 282.461820162796, -355.0948596332293, 282.461820162796, 143.9210226543769, -30.9362945892586, False)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchArc_1.startPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_2.result(), SketchArc_1.results()[1])
SketchArc_2 = Sketch_1.addArc(394.1014919414247, 441.436567364073, 394.1014919414247, -224.6243998437472, -125.0902346435239, 24.21101315681109, True)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_2.startPoint())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchLine_3.result(), SketchArc_2.results()[1])
SketchCircle_1 = Sketch_1.addCircle(-13.45056286489504, 169.4770920976776, 150.6703210346195)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Group_1 = model.addGroup(Part_1_doc, [model.selection("EDGE", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/To_Face_2"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/From_Face_2"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/To_Face_2"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/From_Face_2"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_10&Extrusion_1_1/To_Face_3"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_10&Extrusion_1_1/From_Face_3"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/To_Face_3"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/From_Face_3"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_10"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_9&Extrusion_1_1/Generated_Face_10"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_9&Extrusion_1_1/To_Face_3"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_9&Extrusion_1_1/From_Face_3"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_9"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/Generated_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/To_Face_4"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/From_Face_4"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_5&Extrusion_1_1/From_Face_4"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_5&Extrusion_1_1/To_Face_4"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/Generated_Face_5"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_5&Extrusion_1_1/Generated_Face_6"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_6&Extrusion_1_1/From_Face_4"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_6&Extrusion_1_1/To_Face_4"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_1&Extrusion_1_1/To_Face_4"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_1&Extrusion_1_1/From_Face_4"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_6&Extrusion_1_1/Generated_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_5&Extrusion_1_1/To_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_5&Extrusion_1_1/From_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_11&Extrusion_1_1/Generated_Face_5"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_11&Extrusion_1_1/To_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_11&Extrusion_1_1/From_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_11&Extrusion_1_1/Generated_Face_10"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_6&Extrusion_1_1/Generated_Face_5"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_10&Extrusion_1_1/From_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_10&Extrusion_1_1/To_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_6&Extrusion_1_1/From_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_6&Extrusion_1_1/To_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_7&Extrusion_1_1/Generated_Face_6"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_7&Extrusion_1_1/To_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_7&Extrusion_1_1/From_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_10&Extrusion_1_1/Generated_Face_9"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_9&Extrusion_1_1/From_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_9&Extrusion_1_1/To_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_9&Extrusion_1_1/Generated_Face_8"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_8&Extrusion_1_1/To_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_8&Extrusion_1_1/From_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_8&Extrusion_1_1/Generated_Face_7")])
model.end()

# check that resulting group selection is valid
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Group_1.feature()))
assert(Group_1.groupList().size() == 46)
for a in range(46):
  assert(Group_1.groupList().value(a).value().shapeTypeStr() == "EDGE")
  assert(len(Group_1.groupList().value(a).namingName()) > 0)

assert(model.checkPythonDump())
