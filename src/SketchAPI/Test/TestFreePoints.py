# Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(46.22261352148133, 0, 23.58244120674807)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.result(), SketchCircle_1.center())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchPoint_1 = Sketch_2.addPoint(-17.08115772475531, -35.96860682980034)
SketchLine_2 = Sketch_2.addLine(-21.65537897222015, 0.8259975132940123, 39.97043562830241, -43.0039721165161)
SketchPoint_2 = Sketch_2.addPoint(15.99061885657872, -44.35579681082369)
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchPoint_2.coordinates(), SketchLine_2.result(), 15, True)
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), True)
SketchPoint_3 = SketchProjection_2.createdFeature()
SketchIntersectionPoint_1 = Sketch_2.addIntersectionPoint(model.selection("EDGE", "Sketch_1/SketchCircle_1_2"), False)
[SketchPoint_4, SketchPoint_5] = SketchIntersectionPoint_1.intersectionPoints()
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_4).coordinates(), SketchLine_2.result(), 25, True)
model.do()

FreePoints1 = Sketch_1.getFreePoints()
assert(len(FreePoints1) == 0)

FreePoints2 = Sketch_2.getFreePoints()
assert(len(FreePoints2) == 3)

model.end()
