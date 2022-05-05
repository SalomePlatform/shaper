# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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
    Test1577.py
    Test case for issue #1577 "distance constraint change sign"
"""

from salome.shaper import model
import math

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "Base_Off", "5.875")
model.addParameter(Part_1_doc, "Base_H", "3")
model.addParameter(Part_1_doc, "Base_B", "1.5")
model.addParameter(Part_1_doc, "Wall_R", "6.188")
model.addParameter(Part_1_doc, "Wall_T1", "0.089")
model.addParameter(Part_1_doc, "Wall_T2", "0.261")
model.addParameter(Part_1_doc, "Wall_H", "12.465")
model.addParameter(Part_1_doc, "StrGal_B", "0.75")
model.addParameter(Part_1_doc, "StrGal_H", "1.8")
model.addParameter(Part_1_doc, "StrGal_BU", "0.6")
model.addParameter(Part_1_doc, "StrGal_Inc", "0.2")
model.addParameter(Part_1_doc, "Bracket_H", "1.2")
model.addParameter(Part_1_doc, "Bracket_B", "0.15")
model.addParameter(Part_1_doc, "Bracket_B1", "0.256")
model.addParameter(Part_1_doc, "Bracket_B2", "0.094")
model.addParameter(Part_1_doc, "Bracket_H1", "1.226")
model.addParameter(Part_1_doc, "Cover_R", "9.794")
model.addParameter(Part_1_doc, "Cover_T", "0.082")
model.addParameter(Part_1_doc, "Cover_T1", "0.125")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchPoint_1 = Sketch_1.addPoint(0, 2.425)
SketchConstraintRigid_1 = Sketch_1.setFixed(SketchPoint_1.coordinates())
SketchLine_1 = Sketch_1.addLine(6.277, 12.18846022232796, 6.277, -8.121250994790044)
SketchLine_1.setAuxiliary(True)
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchPoint_1.coordinates(), SketchLine_1.result(), "Wall_R+Wall_T1", False)
SketchLine_2 = Sketch_1.addLine(6.538, -3.45, 7.777, -3.45)
SketchLine_3 = Sketch_1.addLine(7.777, -3.45, 7.777, -6.45)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(7.777, -6.45, 0, -6.45)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(0, -6.45, 0, -3.45)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(0, -3.45, 6.188, -3.45)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchPoint_1.coordinates(), SketchLine_6.result(), "Base_Off", False)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchPoint_1.coordinates(), SketchLine_2.result(), "Base_Off", False)
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchLine_2.endPoint(), SketchLine_4.result(), "Base_H", False)
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchLine_4.startPoint(), SketchLine_1.result(), "Base_B", False)
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchLine_5.startPoint(), SketchLine_1.result(), "Wall_R+Wall_T1", False)
SketchConstraintDistance_7 = Sketch_1.setDistance(SketchLine_6.endPoint(), SketchLine_1.result(), "Wall_T1", False)
SketchConstraintDistance_8 = Sketch_1.setDistance(SketchLine_1.result(), SketchLine_2.startPoint(), "Wall_T2", False)
SketchLine_7 = Sketch_1.addLine(5.677, -4.05, 6.877, -4.05)
SketchLine_8 = Sketch_1.addLine(6.877, -4.05, 7.027, -4.25)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(7.027, -4.25, 7.027, -5.85)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchLine_10 = Sketch_1.addLine(7.027, -5.85, 5.527, -5.85)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchLine_11 = Sketch_1.addLine(5.527, -5.85, 5.527, -4.25)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchLine_12 = Sketch_1.addLine(5.527, -4.25, 5.677, -4.05)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_7.startPoint(), SketchLine_12.endPoint())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_11.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_10.result())
SketchConstraintDistance_9 = Sketch_1.setDistance(SketchLine_10.endPoint(), SketchLine_1.result(), "StrGal_B", False)
SketchConstraintDistance_10 = Sketch_1.setDistance(SketchLine_10.startPoint(), SketchLine_1.result(), "StrGal_B", False)
SketchConstraintDistance_11 = Sketch_1.setDistance(SketchLine_12.endPoint(), SketchLine_1.result(), "StrGal_BU", False)
SketchConstraintDistance_12 = Sketch_1.setDistance(SketchLine_7.endPoint(), SketchLine_1.result(), "StrGal_BU", False)
SketchConstraintDistance_13 = Sketch_1.setDistance(SketchLine_7.endPoint(), SketchLine_10.result(), "StrGal_H", False)
SketchConstraintDistance_14 = Sketch_1.setDistance(SketchLine_7.endPoint(), SketchLine_2.result(), 0.6, True)
SketchConstraintDistance_15 = Sketch_1.setDistance(SketchLine_9.startPoint(), SketchLine_7.result(), "StrGal_Inc", False)
SketchConstraintDistance_16 = Sketch_1.setDistance(SketchLine_11.endPoint(), SketchLine_7.result(), "StrGal_Inc", False)
SketchLine_13 = Sketch_1.addLine(6.188, -3.45, 6.188, 0.287301537219)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_13.startPoint())
SketchLine_14 = Sketch_1.addLine(6.538, -3.45, 6.538, -1.380737101082)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_14.startPoint())
SketchLine_15 = Sketch_1.addLine(6.538, -1.380737101082, 6.366, -1.102730661365)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchLine_16 = Sketch_1.addLine(6.366, -1.102730661365, 6.366, 0.21779992729)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_13.result())
SketchConstraintVertical_7 = Sketch_1.setVertical(SketchLine_14.result())
SketchConstraintVertical_8 = Sketch_1.setVertical(SketchLine_16.result())
SketchConstraintDistance_17 = Sketch_1.setDistance(SketchLine_16.endPoint(), SketchLine_1.result(), "Wall_T1", False)
model.do()

# store coordinates of the gallery
Lines = [SketchLine_7, SketchLine_8, SketchLine_9, SketchLine_10, SketchLine_11, SketchLine_12]
RefCoorindates = []
for line in Lines:
    RefCoorindates.append(line.startPoint().x())
    RefCoorindates.append(line.startPoint().y())
    RefCoorindates.append(line.endPoint().x())
    RefCoorindates.append(line.endPoint().y())

SketchConstraintDistance_18 = Sketch_1.setDistance(SketchLine_14.endPoint(), SketchLine_2.result(), "Wall_H", True)
model.do()

# check coordinates of lines are not changed
TOLERANCE = 1e-6
index = 0
for line in Lines:
    assert(math.fabs(line.startPoint().x() - RefCoorindates[index]) < TOLERANCE)
    assert(math.fabs(line.startPoint().y() - RefCoorindates[index + 1]) < TOLERANCE)
    assert(math.fabs(line.endPoint().x() - RefCoorindates[index + 2]) < TOLERANCE)
    assert(math.fabs(line.endPoint().y() - RefCoorindates[index + 3]) < TOLERANCE)
    index += 4

model.end()
