# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(7.823413936076148e-32, 6, 18, 6)
SketchLine_2 = Sketch_1.addLine(18, 6, 18, 10)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(18, 10, 63, 10)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(63, 10, 63, 8.999999999999995)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(63, 8.999999999999995, 66, 8.999999999999995)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(66, 8.999999999999995, 66, 18)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(66, 18, 70, 18)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(70, 18, 70, 6.999999999999999)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(70, 6.999999999999999, 38, 6.999999999999999)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchLine_10 = Sketch_1.addLine(38, 6.999999999999999, 34, 1.232595164407831e-32)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchLine_11 = Sketch_1.addLine(34, 1.232595164407831e-32, 7.823413936076148e-32, 1.232595164407831e-32)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchLine_12 = Sketch_1.addLine(7.823413936076148e-32, 1.232595164407831e-32, 7.823413936076148e-32, 6)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_12.endPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_11.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_9.result())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintHorizontal_6 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_12.result())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_8.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_12.startPoint(), SketchLine_8.result(), 70, True)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 18)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_4.startPoint(), SketchLine_5.result(), 1, True)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_5.result(), 3)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_7.result(), 4)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_7.endPoint(), SketchLine_11.result(), 18, True)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_12.result(), 6)
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchLine_12.startPoint(), SketchLine_3.result(), 10, True)
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchLine_11.startPoint(), SketchLine_8.result(), 36, True)
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchLine_11.startPoint(), SketchLine_9.result(), 7, True)
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_9.result(), 32)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_12.startPoint(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchLine_13 = Sketch_1.addLine(0, 0, 70, 0)
SketchLine_13.setAuxiliary(True)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_13.startPoint(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchConstraintHorizontal_7 = Sketch_1.setHorizontal(SketchLine_13.result())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_8.result())
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_12r-SketchLine_11r-SketchLine_10r-SketchLine_9r-SketchLine_8r-SketchLine_7r-SketchLine_6r-SketchLine_5r-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection("EDGE", "Sketch_1/SketchLine_13"), 360, 0)
model.do()
# check that naming of the revolution is correct
model.testHaveNamingSubshapes(Revolution_1, model, Part_1_doc)
model.end()
