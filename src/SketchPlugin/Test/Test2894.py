# Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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
    Test2894.py
    Test case for issue #2894 "SIGSEGV in trim after double-click"
"""

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchLine_1 = Sketch_1.addLine(5.899967103122234, 12, -6.100032896877766, 12)
SketchLine_2 = Sketch_1.addLine(-6.100032896877766, 12, -6.100032896877766, 7.282829028407098)
SketchLine_3 = Sketch_1.addLine(5.899967103122234, 8.178211638264205, 5.899967103122234, 12)
SketchLine_3.setName("SketchLine_4")
SketchLine_3.result().setName("SketchLine_4")
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 12)
SketchArc_1 = Sketch_1.addArc(0, 0, -6.100032896877766, 7.282829028407098, -5.830951894845431, -7.499999999999899, False)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchArc_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 9.5)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchArc_1.startPoint())
SketchLine_4 = Sketch_1.addLine(11, -7.5, 5.830951894845414, -7.49999999999991)
SketchLine_4.setName("SketchLine_5")
SketchLine_4.result().setName("SketchLine_5")
SketchLine_5 = Sketch_1.addLine(-11, -7.5, -11, -10)
SketchLine_5.setName("SketchLine_6")
SketchLine_5.result().setName("SketchLine_6")
SketchLine_6 = Sketch_1.addLine(-11, -10, 11, -10)
SketchLine_6.setName("SketchLine_7")
SketchLine_6.result().setName("SketchLine_7")
SketchLine_7 = Sketch_1.addLine(11, -10, 11, -7.5)
SketchLine_7.setName("SketchLine_8")
SketchLine_7.result().setName("SketchLine_8")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_5.result(), 2.5)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_6.result(), 22)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_2.startPoint(), SketchLine_6.result(), 22, True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_6.result(), 10, True)
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_5.startPoint(), SketchAPI_Point(SketchPoint_1).coordinates(), 11)
SketchArc_2 = Sketch_1.addArc(0, 0, 5.830951894845414, -7.49999999999991, 7.627518755950849, 5.663122603973648, False)
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchArc_1.results()[1], SketchArc_2.results()[1])
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchArc_1.center(), SketchArc_2.center())
SketchLine_8 = Sketch_1.addLine(-5.830951894845431, -7.499999999999899, -11, -7.5)
SketchLine_8.setName("SketchLine_9")
SketchLine_8.result().setName("SketchLine_9")
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCollinear_1 = Sketch_1.setCollinear(SketchLine_4.result(), SketchLine_8.result())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_4.endPoint())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_8.startPoint())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_9 = SketchProjection_2.createdFeature()
SketchLine_9.setName("SketchLine_11")
SketchLine_9.result().setName("SketchLine_11")
SketchLine_10 = Sketch_1.addLine(7.627518755950849, 5.663122603973648, 3.634350614531546, 1.669954462554345)
SketchLine_10.setName("SketchLine_13")
SketchLine_10.result().setName("SketchLine_13")
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_10.startPoint(), SketchArc_2.endPoint())
SketchLine_11 = Sketch_1.addLine(1.446475147749155, 3.724719682891128, 5.899967103122234, 8.178211638264205)
SketchLine_11.setName("SketchLine_14")
SketchLine_11.result().setName("SketchLine_14")
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchLine_11.endPoint())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_11.result(), SketchLine_10.result())
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_11.endPoint(), SketchLine_10.result(), 3, True)
SketchLine_12 = Sketch_1.addLine(0, 0, 6.816423472140706, 6.816423472140705)
SketchLine_12.setName("SketchLine_12")
SketchLine_12.result().setName("SketchLine_12")
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_12.startPoint())
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_9.result(), SketchLine_12.result(), 45)
SketchConstraintParallel_2 = Sketch_1.setParallel(SketchLine_12.result(), SketchLine_11.result())
SketchCircle_1 = Sketch_1.addCircle(0, 0, 4)
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchCircle_1.center())
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchCircle_1.results()[1], 4)
model.do()

# Perform first trim
SketchTrim = Sketch_1.addTrim(SketchCircle_1, GeomAPI.GeomAPI_Pnt2d(3.31110220786, 2.24423754741))
SketchTrim.execute()
model.do()

# Check trim does not crash if the selected point is not in specified edge
aLastArc = model.lastSubFeature(Sketch_1, "SketchArc")
SketchTrim = Sketch_1.addTrim(aLastArc, GeomAPI.GeomAPI_Pnt2d(3.31110220786, 2.24423754741))
SketchTrim.execute()
model.do()

model.end()

assert(model.checkPythonDump())
