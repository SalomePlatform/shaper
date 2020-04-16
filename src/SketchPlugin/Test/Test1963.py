# Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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

"""
    Test1963.py
    Test case for issue #1963 "Conflicting constraint wrongly raised"
"""

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(6.188, 9.263, 6.188, -3.45)
SketchLine_2 = Sketch_1.addLine(6.188, -3.45, 0, -3.45)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(0, -3.45, 0, -6.45)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(0, -6.45, 10.31630157312683, -6.45)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(10.31630157312683, -6.45, 10.31630157312683, -3.45)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(10.31630157312683, -3.45, 6.538, -3.45)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_2.result())
SketchLine_7 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OZ"))
SketchLine_7.setName("SketchLine_8")
SketchLine_7.result().setName("SketchLine_8")
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchLine_7.result())
SketchLine_8 = Sketch_1.addLine(6.188, 9.263, 6.376, 9.263)
SketchLine_8.setName("SketchLine_9")
SketchLine_8.result().setName("SketchLine_9")
SketchLine_8.setAuxiliary(True)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_8.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_7.startPoint(), SketchLine_1.result(), 6.188)
SketchLine_9 = Sketch_1.addLine(6.376, 9.263, 6.376, -2.284019283906062)
SketchLine_9.setName("SketchLine_10")
SketchLine_9.result().setName("SketchLine_10")
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchLine_10 = Sketch_1.addLine(6.376, -2.284019283906062, 6.538, -2.359739840005481)
SketchLine_10.setName("SketchLine_11")
SketchLine_10.result().setName("SketchLine_11")
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchLine_11 = Sketch_1.addLine(6.538, -2.359739840005481, 6.538, -3.45)
SketchLine_11.setName("SketchLine_12")
SketchLine_11.result().setName("SketchLine_12")
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_11.endPoint())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_11.result())
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_1.startPoint(), SketchLine_9.result(), 0.188)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_1.result(), SketchLine_11.endPoint(), 0.35)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), 3)
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchLine_7.startPoint(), SketchLine_2.result(), 3.45)
SketchLine_12 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OX"))
SketchLine_12.setName("SketchLine_13")
SketchLine_12.result().setName("SketchLine_13")
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchLine_1.startPoint(), SketchLine_12.result(), 9.263)
SketchLine_13 = Sketch_1.addLine(7.131011037651645, -4.08833050970321, 5.469691015801997, -4.08833050970321)
SketchLine_13.setName("SketchLine_14")
SketchLine_13.result().setName("SketchLine_14")
SketchLine_14 = Sketch_1.addLine(5.469691015801997, -4.08833050970321, 5.469691015801997, -5.799435189085778)
SketchLine_14.setName("SketchLine_15")
SketchLine_14.result().setName("SketchLine_15")
SketchLine_15 = Sketch_1.addLine(5.469691015801997, -5.799435189085778, 7.131011037651645, -5.799435189085778)
SketchLine_15.setName("SketchLine_16")
SketchLine_15.result().setName("SketchLine_16")
SketchLine_16 = Sketch_1.addLine(7.131011037651645, -5.799435189085778, 7.131011037651645, -4.08833050970321)
SketchLine_16.setName("SketchLine_17")
SketchLine_16.result().setName("SketchLine_17")
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_14.result())
SketchConstraintHorizontal_6 = Sketch_1.setHorizontal(SketchLine_15.result())
SketchConstraintVertical_7 = Sketch_1.setVertical(SketchLine_16.result())
SketchLine_17 = Sketch_1.addLine(6.282, 9.263, 6.282, -4.088330509703209)
SketchLine_17.setName("SketchLine_18")
SketchLine_17.result().setName("SketchLine_18")
SketchLine_17.setAuxiliary(True)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_17.startPoint(), SketchLine_8.result())
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_17.endPoint(), SketchLine_13.result())
SketchConstraintVertical_8 = Sketch_1.setVertical(SketchLine_17.result())
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchLine_17.startPoint(), SketchLine_8.result())
model.do()

# check sketch is correct
assert(Sketch_1.solverError().value() == "")

SketchConstraintMiddle_2 = Sketch_1.setMiddlePoint(SketchLine_17.endPoint(), SketchLine_13.result())
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchLine_17.endPoint(), SketchLine_5.result(), 1.5, True)
model.do()

# check sketch is still correct
assert(Sketch_1.solverError().value() == "")

model.end()
