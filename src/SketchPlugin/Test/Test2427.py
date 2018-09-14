## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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

"""
    Test2427.py
    Test case for issue #2427 "error in sketch solver"
"""

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 0, 9.085455512028094, 9.085455512028094)
SketchLine_1.setAuxiliary(True)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_2.result(), SketchLine_1.result(), 45)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchAPI_Line(SketchLine_2).startPoint())
SketchArc_1 = Sketch_1.addArc(0, 0, 3.427521359192312, 2.062061427864143, 2.06206142786378, 3.427521359191917, True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.center())
SketchLine_3 = Sketch_1.addLine(6, 7.365459931328136, 6, 12)
SketchLine_4 = Sketch_1.addLine(6, 12, -6, 12)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(-6, 12, -6, 7.365459931328136)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 4)
SketchLine_6 = Sketch_1.addLine(-5.8309518948453, -7.5, -11, -7.5)
SketchLine_7 = Sketch_1.addLine(-11, -7.5, -11, -10)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(-11, -10, 11, -10)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(11, -10, 11, -7.5)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchLine_10 = Sketch_1.addLine(11, -7.5, 5.830951894845301, -7.5)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_10.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_8.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_8.result(), 22)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_4.result(), 12)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_4.result(), SketchLine_7.endPoint(), 22, True)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_7.result(), 2.5)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_8.endPoint(), SketchLine_2.result(), 10, True)
SketchPoint_1 = Sketch_1.addPoint(0, -10)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_8.result())
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchLine_8.result(), SketchPoint_1.coordinates())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_11 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_11.result(), SketchPoint_1.coordinates())
SketchLine_12 = Sketch_1.addLine(2.06206142786378, 3.427521359191917, 6, 7.365459931328136)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_3.startPoint())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_12.result(), SketchLine_1.result())
SketchLine_13 = Sketch_1.addLine(3.427521359192312, 2.062061427864143, 8.000256151511394, 5.123075395720331)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_13.startPoint())
SketchArc_2 = Sketch_1.addArc(0, 0, 8.000256151511394, 5.123075395720331, 5.830951894845301, -7.5, True)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_2.center())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_13.endPoint())
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchArc_2.endPoint())
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_2.results()[1], 9.5)
SketchLine_14 = Sketch_1.addLine(3.427521359192312, 2.062061427864143, 2.06206142786378, 3.427521359191917)
SketchLine_14.setAuxiliary(True)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchArc_1.endPoint())
SketchPoint_2 = Sketch_1.addPoint(2.744791393527834, 2.744791393527835)
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_14.result())
SketchConstraintMiddle_2 = Sketch_1.setMiddlePoint(SketchLine_14.result(), SketchPoint_2.coordinates())
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchLine_14.result(), SketchPoint_2.coordinates())
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_1.result())
SketchArc_3 = Sketch_1.addArc(0, 0, -6, 7.365459931328136, -5.8309518948453, -7.5, False)
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchArc_3.startPoint())
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchArc_3.endPoint())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchArc_2.results()[1], SketchArc_3.results()[1])
SketchPoint_3 = Sketch_1.addPoint(0, 12)
SketchConstraintMiddle_3 = Sketch_1.setMiddlePoint(SketchLine_4.result(), SketchPoint_3.coordinates())
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchPoint_3.coordinates(), SketchLine_11.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_7.result(), SketchLine_9.result())
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_6.result(), SketchLine_10.result())
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_5.result())
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_13.endPoint(), SketchLine_12.result(), 3, True)
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchArc_3.center(), SketchLine_1.result())
model.do()
model.end()


# Test 1. Add constraint to make auxiliary line degenerated, then undo
model.begin()
aParallel = Sketch_1.setParallel(SketchLine_1.result(), SketchLine_13.result())
model.do()
model.end()
model.checkSketchErrorDegenerated(Sketch_1)
model.undo()
model.checkSketch(Sketch_1)

# Test 2. Add constraint to make auxiliary line degenerated, then remove such constraint
model.begin()
aParallel = Sketch_1.setParallel(SketchLine_1.result(), SketchLine_13.result())
model.do()
model.checkSketchErrorDegenerated(Sketch_1)
Part_1_doc.removeFeature(aParallel.feature())
model.do()
model.end()
model.checkSketch(Sketch_1)

assert(model.checkPythonDump())
