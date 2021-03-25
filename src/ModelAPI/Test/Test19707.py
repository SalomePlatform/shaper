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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-59.0465954847056, -128.5053642637967, 53.14364970069066, -170.5009266857805)
SketchLine_2 = Sketch_1.addLine(53.14364970069066, -170.5009266857805, 137.6705343466976, -112.5874503884433)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(137.6705343466976, -112.5874503884433, 64.87485797099055, -26.90480166687218)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(64.87485797099055, -26.90480166687218, 3.969957323886124, -72.35673939469255)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(3.969957323886124, -72.35673939469255, -79.2369959888606, -34.8592213882167)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(-79.2369959888606, -34.8592213882167, -59.0465954847056, -128.5053642637967)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_6.endPoint())
SketchLine_7 = Sketch_1.addLine(53.14364970069066, -170.5009266857805, 3.969957323886124, -72.35673939469255)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_7.endPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.startPoint(), SketchLine_1.endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_3"))
SketchCircle_1 = Sketch_2.addCircle(110.5461396183937, -4.403937604408839, 25.15832463432426)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2r")], model.selection(), 10, 0)
model.end()

from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Sketch_2.feature()))
assert(aFactory.validate(Extrusion_2.feature()))

# update the first extrusion to make the second bas plane selection invalid
model.begin()
Extrusion_1.setBase([model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_7f-SketchLine_5f-SketchLine_6f")])
model.end()

# check that the second sketch and extrusion become invalid
assert(len(Sketch_2.results()) == 0)
assert(len(Extrusion_2.results()) == 0)
