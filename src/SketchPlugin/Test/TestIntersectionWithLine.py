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
    TestIntersectionWithLine.py
    Unit test of SketchPlugin_IntersectionPoint class
"""

from GeomAPI import *
from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
ParamD = model.addParameter(Part_1_doc, "D", "40")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 0, 50, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(50, 0, 50, 10)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(50, 10, 40, 10)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(40, 10, 40, 30)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(40, 30, 50, 30)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(50, 30, 50, 40)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(50, 40, 0, 50)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(0, 50, 0, 0)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_8.endPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_8.result())
SketchConstraintCollinear_1 = Sketch_1.setCollinear(SketchLine_2.result(), SketchLine_6.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_2.result(), SketchLine_6.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 50)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_8.result(), 50)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_2.result(), 10)
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_1.startPoint(), SketchLine_4.startPoint(), "D")
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_1.endPoint(), SketchLine_7.startPoint(), 40, True)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_8f")], model.selection(), 10, 0)
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 5, 5, [model.selection("SOLID", "Extrusion_1_1")])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4"))
SketchIntersectionPoint_1 = Sketch_2.addIntersectionPoint(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_1/To_Face]"), False)
[SketchPoint_2] = SketchIntersectionPoint_1.intersectionPoints()
SketchCircle_1 = Sketch_2.addCircle(40, 10, 5)
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_2.setRadius(SketchCircle_1.results()[1], 5)
ExtrusionCut_1.setNestedSketch(Sketch_2)
model.do()

assert(SketchIntersectionPoint_1.feature().results().empty() == False)

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [13])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [66])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [132])
model.testResultsVolumes(ExtrusionCut_1, [20303.650459150881943060085177422])

# change parameter and check intersection validity
ParamD.setValue(35)
model.do();

assert(SketchIntersectionPoint_1.feature().results().empty() == False)

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [13])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [66])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [132])
model.testResultsVolumes(ExtrusionCut_1, [19303.650459150881943060085177422])

# change parameter and check the intersection is not valid now
ParamD.setValue(60)
model.do();
assert(SketchIntersectionPoint_1.feature().results().empty() == True)

# change parameter to initial value and check intersection validity
ParamD.setValue(40)
model.do();
# coincidence of circle center and intersection point is lost on previous step, restore it
[SketchPoint_2] = SketchIntersectionPoint_1.intersectionPoints()
Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchCircle_1.center())
model.do()

assert(SketchIntersectionPoint_1.feature().results().empty() == False)

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [13])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [66])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [132])
model.testResultsVolumes(ExtrusionCut_1, [20303.650459150881943060085177422])

model.end()

assert(model.checkPythonDump())
