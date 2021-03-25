# Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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
    TestIntersectionWithCircle.py
    Unit test of SketchPlugin_IntersectionPoint class
"""

from GeomAPI import *
from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
ParamD = model.addParameter(Part_1_doc, "D", "12")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(21.25834261322606, 21.25834261322606, 0, 0)
SketchLine_2 = Sketch_1.addLine(0, 0, 50, -4.088404617988313e-023)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(50, -4.088404617988313e-023, 50, 50)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(50, 50, 28.74165738677394, 28.74165738677394)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintCollinear_1 = Sketch_1.setCollinear(SketchLine_4.result(), SketchLine_1.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_2.result(), SketchLine_3.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_4.result())
SketchArc_1 = Sketch_1.addArc(31, 19, 21.25834261322606, 21.25834261322606, 28.74165738677394, 28.74165738677394, False)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchArc_1.endPoint())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 50)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 10)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_1.center(), SketchLine_2.result(), 19, True)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchArc_1_2r")], model.selection(), 10, 0)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2"), "D", True)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchIntersectionPoint_1 = Sketch_2.addIntersectionPoint(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_1_2][Extrusion_1_1/To_Face]"), True)
SketchIntersectionPoint_1.result().setName("SketchIntersectionPoint_1")
[SketchPoint_2, SketchPoint_3] = SketchIntersectionPoint_1.intersectionPoints()
SketchCircle_1 = Sketch_2.addCircle(38.14142842854285, 10, 1)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchCircle_1.center(), SketchAPI_Point(SketchPoint_2).coordinates())
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_1.results()[1], 1)
SketchLine_5 = Sketch_2.addLine(23.85857157145715, 10.5, 23.85857157145715, 5)
SketchLine_6 = Sketch_2.addLine(22.85857157145715, 6, 22.85857157145715, 10.5)
SketchLine_7 = Sketch_2.addLine(22.85857157145715, 10.5, 23.85857157145715, 10.5)
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_5.startPoint(), SketchLine_7.endPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_3).coordinates(), SketchLine_5.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchConstraintDistanceHorizontal_1 = Sketch_2.setHorizontalDistance(SketchLine_6.endPoint(), SketchLine_5.startPoint(), 1)
SketchConstraintDistanceVertical_1 = Sketch_2.setVerticalDistance(SketchLine_5.startPoint(), SketchAPI_Point(SketchPoint_3).coordinates(), 0.5)
SketchArc_2 = Sketch_2.addArc(23.85857157145715, 6, 23.85857157145715, 5, 22.85857157145715, 6, True)
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_5.result(), SketchArc_2.center())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchArc_2.endPoint(), SketchLine_6.startPoint())
SketchConstraintDistanceVertical_2 = Sketch_2.setVerticalDistance(SketchAPI_Point(SketchPoint_3).coordinates(), SketchArc_2.startPoint(), 5)
SketchConstraintTangent_1 = Sketch_2.setTangent(SketchArc_2.results()[1], SketchLine_6.result())
model.do()
RevolutionCut_1 = model.addRevolutionCut(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_7r-SketchLine_6r-SketchArc_2_2f-SketchLine_5r")], model.selection("EDGE", "Sketch_2/SketchLine_5"), 360, 0, [model.selection("SOLID", "Extrusion_1_1")])
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2f")], model.selection(), 5, 5, [model.selection("SOLID", "RevolutionCut_1_1")])
model.do()

assert(SketchIntersectionPoint_1.feature().results().size() == 2)

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [13])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [66])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [132])
model.testResultsVolumes(ExtrusionCut_1, [9451.611727849665840039961040020])

# change parameter and check intersection validity
ParamD.setValue(15)
model.do();

assert(SketchIntersectionPoint_1.feature().results().size() == 2)

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [14])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [74])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [148])
model.testResultsVolumes(ExtrusionCut_1, [9451.152473626798382611013948917])

# change parameter and check intersection validity
ParamD.setValue(19)
model.do();

assert(SketchIntersectionPoint_1.feature().results().size() == 2)

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [14])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [74])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [148])
model.testResultsVolumes(ExtrusionCut_1, [9448.046243665688962209969758987])

# change parameter and check intersection validity
ParamD.setValue(23)
model.do();

assert(SketchIntersectionPoint_1.feature().results().size() == 1)

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [10])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [50])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [100])
model.testResultsVolumes(ExtrusionCut_1, [9458.635858820198336616158485413])

# change parameter and check intersection validity
ParamD.setValue(29)
model.do();

assert(SketchIntersectionPoint_1.feature().results().size() == 1)

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [9])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [42])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [84])
model.testResultsVolumes(ExtrusionCut_1, [9463.846034357124153757467865944])

model.end()

assert(model.checkPythonDump())
