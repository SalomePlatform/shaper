# Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-35, -10, 20)
SketchLine_1 = Sketch_1.addLine(-21, 2, 24, 21)
SketchLine_2 = Sketch_1.addLine(24, 21, 20, -30)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchArc_1 = Sketch_1.addArc(25, -30, 42, -34, 13, -17.31142245955048, False)
model.do()

# check error on perpendicularity of circle and arc
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchCircle_1.results()[1], SketchArc_1.results()[1])
model.do()
assert(SketchConstraintPerpendicular_1.feature().error() != "")

# avoid the failure
Part_1_doc.removeFeature(SketchConstraintPerpendicular_1.feature())
model.do()
assert(Sketch_1.feature().error() == "")

# set correct constraints
SketchConstraintPerpendicular_2 = Sketch_1.setPerpendicular(SketchCircle_1.results()[1], SketchLine_1.result())
SketchConstraintPerpendicular_3 = Sketch_1.setPerpendicular(SketchLine_2.result(), SketchArc_1.results()[1])
model.do()

TOLERANCE = 1.e-7
assert(model.distancePointLine(SketchCircle_1.center(), SketchLine_1) < TOLERANCE)
assert(model.distancePointLine(SketchArc_1.center(), SketchLine_2) < TOLERANCE)
model.end()

assert(model.checkPythonDump())
