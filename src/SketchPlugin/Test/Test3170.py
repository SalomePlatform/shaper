# Copyright (C) 2020-2021  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-17.82085345143611, 21.43637119696745, -32.3626718085896, -17.44370962732489)
SketchLine_2 = Sketch_1.addLine(-32.3626718085896, -17.44370962732489, 6.049253461791531, -11.27075409452475)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(6.049253461791531, -11.27075409452475, -17.82085345143611, 21.43637119696745)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
SketchLine_4 = Sketch_1.addLine(-32.3626718085896, -17.44370962732489, -5.812861704098991, 4.982867573918385)
SketchLine_4.setAuxiliary(True)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_3.result())
model.end()

DoF = model.dof(Sketch_1)

model.begin()
Sketch_1.setFillet(SketchLine_2.startPoint())
DoF += 3
model.end()

assert(model.dof(Sketch_1) == DoF)

model.testNbSubFeatures(Sketch_1, "SketchPoint", 0)
model.testNbSubFeatures(Sketch_1, "SketchLine", 4)
model.testNbSubFeatures(Sketch_1, "SketchArc", 1)
model.testNbSubFeatures(Sketch_1, "SketchConstraintCoincidence", 5)
model.testNbSubFeatures(Sketch_1, "SketchConstraintTangent", 2)

assert(model.checkPythonDump())
