# Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))
SketchLine_1 = Sketch_1.addLine(3.455185109181585, 2.504529791754867, 6.714574768930328, 7.155950452021305)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Top]"), True)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), True)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchProjection_3 = Sketch_1.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_1"), True)
SketchProjection_4 = Sketch_1.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex"), True)
model.do()

model.end()

from ModelAPI import *
validators = ModelAPI_Session.get().validators()
assert(validators.validate(SketchProjection_1.feature()))
assert(validators.validate(SketchProjection_2.feature()))
assert(not validators.validate(SketchProjection_3.feature()))
assert(not validators.validate(SketchProjection_4.feature()))
