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
    Test1578.py
    Test case for issue #1578 "arc problems"
"""

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
model.addParameter(partSet, "R_stator_int", "75")
model.addParameter(partSet, "R_stator_ext", "120")
model.addParameter(partSet, "angle", "7.5")
model.addParameter(partSet, "Length_1", "1.6")
model.addParameter(partSet, "Length_2", "2.29")
model.addParameter(partSet, "Length_3", "3.5")
model.addParameter(partSet, "h", "1.3")
model.addParameter(partSet, "h1", "1.9")
model.addParameter(partSet, "h2", "15.49")
model.addParameter(partSet, "R_rotor", "67.5")
model.addParameter(partSet, "R_abre", "29.5")
model.addParameter(partSet, "l_aimant", "37.5")
model.addParameter(partSet, "h_aimant", "64")
model.addParameter(partSet, "R_aimant", "70.5")
model.addParameter(partSet, "l_cut_rotor", "37.92")
model.addParameter(partSet, "h_cut_rotor", "63.8")
model.addParameter(partSet, "R_interm", "72.75")
model.addParameter(partSet, "angle_aimant", "22.5")
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 0, 115.7437109013936, 7.586243611563176)
SketchLine_1.setAuxiliary(True)
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OX"))
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_2.result(), SketchLine_1.result(), "angle/2")
SketchLine_3 = Sketch_1.addLine(0, 0, 101.5823409704896, 13.37356889953941)
SketchLine_3.setAuxiliary(True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchPoint_1.result())
SketchConstraintAngle_2 = Sketch_1.setAngle(SketchLine_2.result(), SketchLine_3.result(), "angle")
SketchArc_1 = Sketch_1.addArc(0, 0, 75, 0, 74.35836460303578, 9.789464416503867, False)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.center(), SketchPoint_1.result())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_2.result())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_3.result())
SketchArc_2 = Sketch_1.addArc(0, 0, 120, 0, 118.9733833648573, 15.66314306640619, False)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_2.center(), SketchPoint_1.result())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_2.result())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_3.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], "R_stator_int")
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_2.results()[1], "R_stator_ext")
SketchLine_4 = Sketch_1.addLine(74.35836460303578, 9.789464416503867, 118.9733833648573, 15.66314306640619)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_4.endPoint())
SketchLine_5 = Sketch_1.addLine(75, 0, 120, 0)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_5.endPoint())
SketchArc_3 = Sketch_1.addArc(0, 0, 76.22453904297332, 3.392587167080116, 76.01524902943686, 6.585735721443648, False)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchArc_3.center(), SketchPoint_1.result())
SketchArc_4 = Sketch_1.addArc(0, 0, 78.14887552638105, 2.827234330967321, 77.84932919450701, 7.397428199400126, False)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchArc_4.center(), SketchPoint_1.result())
SketchArc_5 = Sketch_1.addArc(0, 0, 93.65305549448868, 2.630835712510912, 93.19523358987767, 9.615848175181137, False)
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchArc_5.center(), SketchPoint_1.result())
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchArc_3.results()[1], "R_stator_int+h")
SketchConstraintRadius_4 = Sketch_1.setRadius(SketchArc_4.results()[1], "R_stator_int+h+h1")
SketchConstraintRadius_5 = Sketch_1.setRadius(SketchArc_5.results()[1], "R_stator_int+h+h1+h2")
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_5.endPoint(), SketchLine_1.result(), "Length_3")
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchArc_5.startPoint(), SketchLine_1.result(), "Length_3")
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchArc_4.endPoint(), SketchLine_1.result(), "Length_2")
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchArc_4.startPoint(), SketchLine_1.result(), "Length_2")
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchArc_3.endPoint(), SketchLine_1.result(), "Length_1")
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchArc_3.startPoint(), SketchLine_1.result(), "Length_1")
SketchLine_6 = Sketch_1.addLine(77.84932919450701, 7.397428199400126, 93.19523358987767, 9.615848175181137)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchArc_5.endPoint(), SketchLine_6.endPoint())
SketchLine_7 = Sketch_1.addLine(78.14887552638105, 2.827234330967321, 93.65305549448868, 2.630835712510912)
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchArc_4.startPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchArc_5.startPoint(), SketchLine_7.endPoint())
SketchLine_8 = Sketch_1.addLine(76.01524902943686, 6.585735721443648, 74.71774217241287, 6.500692629007779)
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchArc_1.results()[1])
SketchLine_9 = Sketch_1.addLine(76.22453904297332, 3.392587167080116, 74.92703218594932, 3.307544074644247)
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchArc_3.startPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchArc_1.results()[1])
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_8.result(), SketchLine_1.result())
SketchConstraintParallel_2 = Sketch_1.setParallel(SketchLine_9.result(), SketchLine_1.result())
SketchArc_6 = Sketch_1.addArc(92.99435209423221, 6.095171858515482, 93.19523358987767, 9.615848175181137, 93.65305549448868, 2.630835712510912, True)
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchArc_5.endPoint(), SketchArc_6.startPoint())
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchArc_6.startPoint())
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchArc_5.startPoint(), SketchArc_6.endPoint())
SketchArc_7 = Sketch_1.addArc(78.08372875230802, 4.389817544749706, 76.015249029437, 6.585735721444, 77.84932919450701, 7.397428199400126, True)
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchArc_7.startPoint())
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchLine_8.startPoint(), SketchArc_7.startPoint())
SketchConstraintCoincidence_29 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchArc_7.endPoint())
SketchConstraintCoincidence_30 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchArc_7.endPoint())
SketchArc_8 = Sketch_1.addArc(77.98869779703921, 5.839709741271425, 76.22453904297301, 3.39258716708, 78.14887552638105, 2.827234330967321, False)
SketchConstraintCoincidence_31 = Sketch_1.setCoincident(SketchArc_3.startPoint(), SketchArc_8.startPoint())
SketchConstraintCoincidence_32 = Sketch_1.setCoincident(SketchLine_9.startPoint(), SketchArc_8.startPoint())
SketchConstraintCoincidence_33 = Sketch_1.setCoincident(SketchArc_4.startPoint(), SketchArc_8.endPoint())
SketchConstraintCoincidence_34 = Sketch_1.setCoincident(SketchLine_7.startPoint(), SketchArc_8.endPoint())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchArc_7.results()[1], SketchArc_8.results()[1])
SketchArc_9 = Sketch_1.addArc(0, 0, 78.14887552638101, 2.827234330967, 78.2, 0, True)
SketchConstraintCoincidence_35 = Sketch_1.setCoincident(SketchArc_9.center(), SketchPoint_1.result())
SketchConstraintCoincidence_36 = Sketch_1.setCoincident(SketchArc_4.startPoint(), SketchArc_9.startPoint())
SketchConstraintCoincidence_37 = Sketch_1.setCoincident(SketchLine_7.startPoint(), SketchArc_9.startPoint())
SketchConstraintCoincidence_38 = Sketch_1.setCoincident(SketchArc_8.endPoint(), SketchArc_9.startPoint())
SketchConstraintCoincidence_39 = Sketch_1.setCoincident(SketchArc_9.endPoint(), SketchLine_5.result())
model.do()

model.checkSketch(Sketch_1)

# Check sketch is valid after adding the arc
SketchArc_10 = Sketch_1.addArc(0, 0, 77.84932919450699, 7.397428199400125, 77.5309881595101, 10.20714823162807, False)
SketchConstraintCoincidence_40 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_10.center())
SketchConstraintCoincidence_41 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchArc_10.startPoint())
SketchConstraintCoincidence_42 = Sketch_1.setCoincident(SketchLine_4.result(), SketchArc_10.endPoint())
model.do()

model.checkSketch(Sketch_1)

# Check sketch is valid after multi-rotation
SketchMultiRotation_1_objects = [SketchArc_1.results()[1], SketchLine_9.result(), SketchLine_8.result(), SketchArc_3.results()[1], SketchArc_8.results()[1], SketchArc_7.results()[1], SketchArc_9.results()[1], SketchArc_4.results()[1], SketchArc_10.results()[1], SketchLine_4.result(), SketchLine_7.result(), SketchLine_6.result(), SketchArc_5.results()[1], SketchArc_6.results()[1], SketchArc_2.results()[1]]
SketchMultiRotation_1 = Sketch_1.addRotation(SketchMultiRotation_1_objects, SketchLine_2.startPoint(), "angle", 12)
model.do()

model.checkSketch(Sketch_1)

# Move arc's extremity and check sketch
SketchArc_121 = Sketch_1.addArc(0, 0, 36.41124276672319, 6.344317038870018, 8.424960444516818, 35.98678924293296, False)
SketchConstraintCoincidence_43 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_121.center())
model.do()

for i in range(0, 10):
    Sketch_1.move(SketchArc_121.startPoint(), SketchArc_121.startPoint().x() + 1., SketchArc_121.startPoint().y())
    model.do()
    model.checkSketch(Sketch_1)

model.end()
