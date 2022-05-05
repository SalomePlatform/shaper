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
    Test2119.py
    Test case for issue #2119 "Sketch lost when creating a tangent constraint"
"""

from salome.shaper import model

from ModelAPI import *
from SketchAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "a", "63")
model.addParameter(Part_1_doc, "b", "50")
model.addParameter(Part_1_doc, "c", "100")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(-45, 0, 0, 0)
SketchLine_2 = Sketch_1.addLine(0, 0, 23.33965093306752, 8.494938217797719)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(33.83092017818969, 6.16907982180411, 40, 0)
SketchLine_4 = Sketch_1.addLine(40, 0, 55, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(55, 0, 55, 48)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(55, 48, 15, 48)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(15, 48, 15, 63)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(15, 63, -15, 63)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(-15, 63, -15, 35)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_8.result())
SketchConstraintCollinear_1 = Sketch_1.setCollinear(SketchLine_1.result(), SketchLine_4.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_3.endPoint(), SketchLine_4.endPoint(), 15, False)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_3.endPoint(), SketchLine_1.endPoint(), 40, False)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_1.startPoint(), SketchLine_4.endPoint(), "c", False)
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchLine_8.startPoint(), SketchLine_6.result(), 15, False)
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchLine_8.startPoint(), SketchLine_5.result(), 40, False)
SketchLine_10 = Sketch_1.addLine(-45, 5, -45, 0)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_10.endPoint())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_10.result())
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchLine_9.startPoint(), SketchLine_1.result(), "a", False)
SketchArc_1 = Sketch_1.addArc(-45, 35, -45, 5, -15, 35, False)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_10.result())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_10.startPoint())
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_3.result(), SketchLine_1.result(), 45)
SketchConstraintAngle_2 = Sketch_1.setAngleBackward(SketchLine_2.result(), SketchLine_4.result(), 20)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_9.endPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_9.result())
SketchConstraintDistance_7 = Sketch_1.setDistance(SketchArc_1.center(), SketchLine_1.result(), 35, False)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 30)
SketchArc_2 = Sketch_1.addArc(26.75985236632421, -0.9019879900613652, 33.83092017818969, 6.16907982180411, 23.33965093306752, 8.494938217797719, False)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_2.endPoint())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_2.result())
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_3.result())
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_2.results()[1], 10)
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.coordinates())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchArc_2_2r-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_8f-SketchLine_9f-SketchArc_1_2r-SketchLine_10f")], model.selection(), "b/2", "b/2")
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchProjection_1 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_9][Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_1_2][Extrusion_1_1/To_Face]"), False)
SketchPoint_2 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_2.addCircle(-15, 35, 25)
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchCircle_1.center(), SketchPoint_2.result())
SketchConstraintRadius_3 = Sketch_2.setRadius(SketchCircle_1.results()[1], "50/2")
model.do()
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchCircle_1_2f_wire")], model.selection(), 0, 13, [model.selection("SOLID", "Extrusion_1_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face"))
SketchProjection_2 = Sketch_3.addProjection(model.selection("VERTEX", "ExtrusionCut_1_1/Generated_Vertex&ExtrusionCut_1_1/From_Face"), False)
SketchPoint_3 = SketchProjection_2.createdFeature()
SketchProjection_3 = Sketch_3.addProjection(model.selection("EDGE", "([ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchArc_1_2][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_9])([Extrusion_1_1/From_Face][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_9])2([Extrusion_1_1/From_Face][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchArc_1_2])2([ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/To_Face][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_9])2"), False)
SketchLine_11 = SketchProjection_3.createdFeature()
SketchArc_3 = Sketch_3.addArc(-15, 35, -15, 46.0103340429751, -17.02045759563166, 24.17663606626138, True)
SketchConstraintCoincidence_16 = Sketch_3.setCoincident(SketchPoint_3.result(), SketchArc_3.center())
SketchConstraintCoincidence_17 = Sketch_3.setCoincident(SketchLine_11.result(), SketchArc_3.startPoint())
SketchProjection_4 = Sketch_3.addProjection(model.selection("EDGE", "([ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchArc_1_2][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_9])([Extrusion_1_1/From_Face][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_9])2([Extrusion_1_1/From_Face][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchArc_1_2])2([ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/To_Face][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchArc_1_2])2"), False)
SketchArc_4 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_18 = Sketch_3.setCoincident(SketchArc_3.endPoint(), SketchArc_4.results()[1])
SketchLine_12 = Sketch_3.addLine(-15, 46.0103340429751, -15, 35)
SketchConstraintCoincidence_19 = Sketch_3.setCoincident(SketchArc_3.startPoint(), SketchLine_12.startPoint())
SketchArc_5 = Sketch_3.addArc(-24.92022800465918, 31.2515971841183, -15, 35, -17.02045759563166, 24.17663606626138, True)
SketchConstraintCoincidence_20 = Sketch_3.setCoincident(SketchAPI_Point(SketchPoint_3).coordinates(), SketchArc_5.startPoint())
SketchConstraintCoincidence_21 = Sketch_3.setCoincident(SketchArc_3.endPoint(), SketchArc_5.endPoint())
model.do()
model.end()


# case 1: undo the error
model.begin()
aTangent = Sketch_3.setTangent(SketchArc_5.results()[1], SketchArc_4.results()[1])
model.do()
model.end()
model.checkSketchErrorDegenerated(Sketch_3)
model.undo()
model.checkSketch(Sketch_3)

# case 2: remove degeneracy-producting constraint
model.begin()
aTangent = Sketch_3.setTangent(SketchArc_5.results()[1], SketchArc_4.results()[1])
model.do()
model.checkSketchErrorDegenerated(Sketch_3)
Part_1_doc.removeFeature(aTangent.feature())
model.do()
model.end()
model.checkSketch(Sketch_3)

# case 3: remove degenerated edge
model.begin()
aTangent = Sketch_3.setTangent(SketchArc_5.results()[1], SketchArc_4.results()[1])
model.do()
model.checkSketchErrorDegenerated(Sketch_3)
ModelAPI.removeFeaturesAndReferences(FeatureSet([SketchArc_5.feature()]))
model.do()
model.end()
model.checkSketch(Sketch_3)

assert(model.checkPythonDump())
