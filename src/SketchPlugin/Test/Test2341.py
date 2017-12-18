## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
    Test2341.py
    Test case for issue #2341 "SIGSEGV when trying to move a point of a projected axis"
"""

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-5.830951894848558, -7.5, -11, -7.5)
SketchLine_2 = Sketch_1.addLine(-11, -7.5, -11, -10)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-11, -10, 11, -10)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(11, -10, 11, -7.5)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(11, -7.5, 5.8309518948453, -7.5)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 2.5)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_4.result(), 2.5)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_3.result(), 22)
SketchConstraintLength_3.setName("SketchConstraintLength_5")
SketchLine_6 = Sketch_1.addLine(-6, 12, 6, 12)
SketchLine_7 = Sketch_1.addLine(6, 12, 6, 8.121320343559642)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(-6, 12, -6, 7.365459931328117)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_6.result(), 12)
SketchConstraintLength_4.setName("SketchConstraintLength_6")
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchProjection_3 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_3 = SketchProjection_3.createdFeature()
SketchProjection_4 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_4 = SketchProjection_4.createdFeature()
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchAPI_Point(SketchPoint_4).coordinates(), SketchLine_4.startPoint(), 10)
SketchProjection_5 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_5 = SketchProjection_5.createdFeature()
SketchArc_1 = Sketch_1.addArc(0, 0, -5.830951894848558, -7.5, -6, 7.365459931328117, True)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchPoint_5.result(), SketchArc_1.center())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.startPoint())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 9.5)
SketchProjection_6 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_6 = SketchProjection_6.createdFeature()
SketchArc_2 = Sketch_1.addArc(0, 0, 5.8309518948453, -7.5, 7.693909752490621, 5.572589408930978, False)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchPoint_6.result(), SketchArc_2.center())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchArc_2.startPoint())
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_2.results()[1], 9.5)
SketchArc_3 = Sketch_1.addArc(0, 0, 1.561361948645558, 3.6826822922052, 3.682682292205201, 1.561361948645557, False)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchArc_3.center())
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchArc_3.results()[1], 4)
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_6.endPoint(), SketchArc_1.center(), 6)
SketchConstraintDistanceHorizontal_2 = Sketch_1.setHorizontalDistance(SketchLine_3.endPoint(), SketchArc_1.center(), 11)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchArc_1.endPoint())
SketchConstraintDistanceVertical_2 = Sketch_1.setVerticalDistance(SketchLine_8.startPoint(), SketchLine_2.endPoint(), 22)
SketchLine_9 = Sketch_1.addLine(0, 0, 14.14213562373095, 14.14213562373095)
SketchLine_9.setAuxiliary(True)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_9.startPoint())
SketchProjection_7 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_10 = SketchProjection_7.createdFeature()
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_10.result(), SketchLine_9.result(), 45)
SketchLine_11 = Sketch_1.addLine(7.693909752490621, 5.572589408930978, 3.682682292205201, 1.561361948645557)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_11.endPoint())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_9.result(), SketchLine_11.result())
SketchLine_12 = Sketch_1.addLine(6, 8.121320343559642, 1.561361948645558, 3.6826822922052)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchArc_3.startPoint(), SketchLine_12.endPoint())
SketchConstraintParallel_2 = Sketch_1.setParallel(SketchLine_9.result(), SketchLine_12.result())
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_12.startPoint(), SketchLine_7.endPoint())
SketchLine_13 = Sketch_1.addLine(3.682682292205201, 1.561361948645557, 1.561361948645558, 3.6826822922052)
SketchLine_13.setAuxiliary(True)
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchArc_3.startPoint(), SketchLine_13.endPoint())
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_13.result(), 3)
SketchConstraintLength_5.setName("SketchConstraintLength_7")
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_3.endPoint(), SketchLine_9.result(), 1.5, True)
SketchConstraintLength_6 = Sketch_1.setLength(SketchLine_9.result(), 20)
SketchConstraintLength_6.setName("SketchConstraintLength_8")
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1f-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchLine_6r-SketchLine_7r-SketchLine_8f-SketchArc_1_2f-SketchArc_2_2f-SketchArc_3_2r-SketchLine_11f-SketchLine_12r")], model.selection(), 3.5, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face_3"))
SketchLine_14 = Sketch_2.addLine(6, 0, -6, 0)
SketchProjection_8 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/Vertex-SketchLine_7s-SketchLine_6e"), False)
SketchPoint_7 = SketchProjection_8.createdFeature()
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchLine_14.startPoint(), SketchPoint_7.result())
SketchProjection_9 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/Vertex-SketchLine_6s-SketchLine_8s"), False)
SketchPoint_8 = SketchProjection_9.createdFeature()
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchPoint_8.result())
SketchLine_15 = Sketch_2.addLine(-6, 0, -6, -3.5)
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchLine_16 = Sketch_2.addLine(-6, -3.5, -6, -10.5)
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchLine_17 = Sketch_2.addLine(5.978225648847096, 0.08643259872215214, 6, -3.5)
SketchProjection_10 = Sketch_2.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_3&Extrusion_1_1/To_Face_1"), False)
SketchPoint_9 = SketchProjection_10.createdFeature()
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchLine_17.endPoint(), SketchPoint_9.result())
SketchLine_18 = Sketch_2.addLine(6, -3.5, 6, -10.5)
SketchConstraintCoincidence_25 = Sketch_2.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
SketchConstraintLength_7 = Sketch_2.setLength(SketchLine_18.result(), 7)
SketchConstraintLength_7.setName("SketchConstraintLength_9")
SketchProjection_11 = Sketch_2.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_2&Extrusion_1_1/To_Face_1"), False)
SketchPoint_10 = SketchProjection_11.createdFeature()
SketchConstraintCoincidence_26 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_10).coordinates(), SketchLine_15.endPoint())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_18.result())
SketchConstraintVertical_6 = Sketch_2.setVertical(SketchLine_16.result())
SketchLine_19 = Sketch_2.addLine(6, -10.5, -6, -10.5)
SketchLine_19.setAuxiliary(True)
SketchConstraintCoincidence_27 = Sketch_2.setCoincident(SketchLine_19.startPoint(), SketchLine_18.endPoint())
SketchConstraintCoincidence_28 = Sketch_2.setCoincident(SketchLine_19.endPoint(), SketchLine_16.endPoint())
SketchConstraintEqual_1 = Sketch_2.setEqual(SketchLine_18.result(), SketchLine_16.result())
SketchProjection_12 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_20 = SketchProjection_12.createdFeature()
model.do()

SketchLine_20 = SketchAPI_Line(SketchLine_20)
lineStart = [SketchLine_20.startPoint().x(), SketchLine_20.startPoint().y()]
lineEnd = [SketchLine_20.endPoint().x(), SketchLine_20.endPoint().y()]

# Try to move a line provided by projection of OZ axis

# move start point
Sketch_2.move(SketchLine_20.startPoint(), lineStart[0] + 1., lineStart[1] + 1.)
model.do()
assert(SketchLine_20.startPoint().x() == lineStart[0] and SketchLine_20.startPoint().y() == lineStart[1])

# move end point
Sketch_2.move(SketchLine_20.endPoint(), lineEnd[0] + 1., lineEnd[1] + 1.)
model.do()
assert(SketchLine_20.endPoint().x() == lineEnd[0] and SketchLine_20.endPoint().y() == lineEnd[1])

# move whole line
Sketch_2.move(SketchLine_20.result(), 50., 50.)
model.do()
assert(SketchLine_20.startPoint().x() == lineStart[0] and SketchLine_20.startPoint().y() == lineStart[1])
assert(SketchLine_20.endPoint().x() == lineEnd[0] and SketchLine_20.endPoint().y() == lineEnd[1])

model.end()
