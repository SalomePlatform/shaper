# Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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
from salome.shaper import geom

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(12, -8, -12, -8)
SketchLine_2 = Sketch_1.addLine(-12, -8, -12, 20)
SketchLine_3 = Sketch_1.addLine(-12, 20, 12, 20)
SketchLine_4 = Sketch_1.addLine(12, 20, 12, -8)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchArc_1 = Sketch_1.addArc(0, -3, 20, -3, -20, -3, False)
SketchLine_5 = Sketch_1.addLine(20, -3, 20, -30)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(20, 17, 50, 17)
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_5.result(), SketchArc_1.results()[1])
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchLine_6.result(), SketchArc_1.results()[1])
model.do()

model.testNbSubFeatures(Sketch_1, "SketchArc", 1)
model.testNbSubFeatures(Sketch_1, "SketchLine", 6)
model.testNbSubFeatures(Sketch_1, "SketchConstraintCoincidence", 5)
model.testNbSubFeatures(Sketch_1, "SketchConstraintTangent", 2)
model.testNbSubFeatures(Sketch_1, "SketchConstraintEqual", 0)

# perform trim
SketchTrim = Sketch_1.addTrim(SketchArc_1, geom.Pnt2d(0, 17))
SketchTrim.execute()
model.do()

model.testNbSubFeatures(Sketch_1, "SketchArc", 2)
model.testNbSubFeatures(Sketch_1, "SketchLine", 6)
model.testNbSubFeatures(Sketch_1, "SketchConstraintCoincidence", 8)
model.testNbSubFeatures(Sketch_1, "SketchConstraintTangent", 2)
model.testNbSubFeatures(Sketch_1, "SketchConstraintEqual", 1)

model.end()

assert(model.checkPythonDump())
