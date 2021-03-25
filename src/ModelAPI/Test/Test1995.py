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

# Lighter case to reproduce problem of #1995
# The problem was in axis that contained sub-shapes naming of the sketch and the extrusion becomes
# use this axis. So, on remove of the axis, the naming is failed. Same for modifications of the sketch 1.

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(175.5289977047663, 37.215818549061, -134.623385345167, 37.215818549061)
SketchLine_2 = Sketch_1.addLine(-134.623385345167, 37.215818549061, -134.623385345167, -108.7867760111381)
SketchLine_3 = Sketch_1.addLine(-134.623385345167, -108.7867760111381, 175.5289977047663, -108.7867760111381)
SketchLine_4 = Sketch_1.addLine(175.5289977047663, -108.7867760111381, 175.5289977047663, 37.215818549061)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Axis_4 = model.addAxis(Part_1_doc, model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f"), model.selection("VERTEX", "Sketch_1/SketchLine_2_EndVertex"))
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 50, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3"))
SketchCircle_1 = Sketch_2.addCircle(12.88847795756506, 28.93359954982685, 11.55225986344597)
model.do()

# now remove axis to destroy naming structure if extrusion refers to it
Part_1_doc.removeFeature(Axis_4.feature())
# also update sketch_1 to launch recomputation of everything
SketchLine_1.setStartPoint(170, -130)
model.end()

import ModelAPI
assert(ModelAPI.ModelAPI_Session.get().validators().validate(Sketch_2.feature()))
