# Copyright (C) 2018-2023  CEA/DEN, EDF R&D
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

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchLine_3 = Sketch_1.addLine(60, 10, 0, 0)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).startPoint(), SketchLine_3.endPoint())
SketchLine_4 = Sketch_1.addLine(20, 50, 0, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).startPoint(), SketchLine_4.endPoint())
model.do()

SketchFillet_1 = Sketch_1.setFillet(SketchLine_3.endPoint())
model.do()

assert(Sketch_1.feature().error() == "")
assert(Sketch_1.solverError().value() == "")

model.end()
