# Copyright (C) 2018-2019  CEA/DEN, EDF R&D
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
    TestIntersectionWithSpline.py
    Unit test of SketchPlugin_IntersectionPoint class
"""

from GeomAPI import *
from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
ParamD = model.addParameter(Part_1_doc, "D", "5")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchArc_1 = Sketch_1.addArc(-20, 0, -12.51615726667241, -49.43674845641531, -67.51615726667241, 15.56325154358469, False)
SketchLine_1 = Sketch_1.addLine(-67.51615726667241, 15.56325154358469, -12.51615726667241, 15.56325154358469)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.startPoint())
SketchLine_2 = Sketch_1.addLine(-12.51615726667241, 15.56325154358469, -12.51615726667241, -49.43674845641531)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_2.endPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_3 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_3.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 50)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 65)
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchArc_1.center(), SketchAPI_Line(SketchLine_3).startPoint(), 20)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_1.result(), 55)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_1_2r-SketchLine_1f-SketchLine_2f")], model.selection(), 25, 25)
Filling_1 = model.addFilling(Part_1_doc, [model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/From_Face]"), model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_1_2][Extrusion_1_1/To_Face]")])
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_1_2r-SketchLine_1f-SketchLine_2f")])
Intersection_1 = model.addIntersection(Part_1_doc, [model.selection("FACE", "Filling_1_1"), model.selection("FACE", "Face_1_1")])
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_4 = Sketch_2.addLine(-34.11217447219568, 40, 20, -37.28019411294778)
SketchLine_5 = Sketch_2.addLine(20, -37.28019411294778, 20, 40)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_2.addLine(20, 40, -34.11217447219568, 40)
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_4.startPoint(), SketchLine_6.endPoint())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_5.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_6.result())
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_2.createdFeature()
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_5.result(), 20, True)
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_6.result(), 40, True)
SketchConstraintDistance_3 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_4.result(), "D", True)
SketchConstraintAngle_1 = Sketch_2.setAngle(SketchLine_5.result(), SketchLine_4.result(), 35)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_4r-SketchLine_5f-SketchLine_6f")], model.selection(), 10, 10)
ExtrusionFuse_1 = model.addExtrusionFuse(Part_1_doc, [], model.selection(), 10, 0, [model.selection("SOLID", "Extrusion_2_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_4"))
SketchIntersectionPoint_1 = Sketch_3.addIntersectionPoint(model.selection("EDGE", "Intersection_1_1"), False)
[SketchPoint_2, SketchPoint_3] = SketchIntersectionPoint_1.intersectionPoints()
SketchCircle_1 = Sketch_3.addCircle(-33.22640570408476, 0, 5)
SketchConstraintCoincidence_8 = Sketch_3.setCoincident(SketchCircle_1.center(), SketchAPI_Point(SketchPoint_2).coordinates())
SketchCircle_2 = Sketch_3.addCircle(21.47998214403209, 0, 5)
SketchConstraintCoincidence_9 = Sketch_3.setCoincident(SketchCircle_2.center(), SketchAPI_Point(SketchPoint_3).coordinates())
SketchConstraintEqual_1 = Sketch_3.setEqual(SketchCircle_1.results()[1], SketchCircle_2.results()[1])
SketchConstraintRadius_2 = Sketch_3.setRadius(SketchCircle_1.results()[1], 5)
ExtrusionFuse_1.setNestedSketch(Sketch_3)
model.do()

assert(SketchIntersectionPoint_1.feature().results().size() == 2)

model.testNbResults(ExtrusionFuse_1, 1)
model.testNbSubResults(ExtrusionFuse_1, [0])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.FACE, [9])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.EDGE, [30])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.VERTEX, [60])
model.testResultsVolumes(ExtrusionFuse_1, [43388.789797644698410294950008392])

# change parameter and check intersection validity
ParamD.setValue(15)
model.do();

assert(SketchIntersectionPoint_1.feature().results().size() == 2)

model.testNbResults(ExtrusionFuse_1, 1)
model.testNbSubResults(ExtrusionFuse_1, [0])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.FACE, [9])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.EDGE, [30])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.VERTEX, [60])
model.testResultsVolumes(ExtrusionFuse_1, [64385.484780074148147832602262497])

# change parameter and check single intersection
ParamD.setValue(25)
model.do();

assert(SketchIntersectionPoint_1.feature().results().size() == 1)

model.testNbResults(ExtrusionFuse_1, 1)
model.testNbSubResults(ExtrusionFuse_1, [0])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.FACE, [9])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.EDGE, [30])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.VERTEX, [60])
model.testResultsVolumes(ExtrusionFuse_1, [89638.890852351003559306263923645])

# change parameter and check intersection is NOT valid now
ParamD.setValue(50)
model.do();

assert(SketchIntersectionPoint_1.feature().results().empty())

# revert parameter to original value and reset coincidences
ParamD.setValue(5)
model.do()
[SketchPoint_2, SketchPoint_3] = SketchIntersectionPoint_1.intersectionPoints()
SketchConstraintCoincidence_8 = Sketch_3.setCoincident(SketchCircle_1.center(), SketchAPI_Point(SketchPoint_2).coordinates())
SketchConstraintCoincidence_9 = Sketch_3.setCoincident(SketchCircle_2.center(), SketchAPI_Point(SketchPoint_3).coordinates())
model.do()

assert(SketchIntersectionPoint_1.feature().results().size() == 2)

model.testNbResults(ExtrusionFuse_1, 1)
model.testNbSubResults(ExtrusionFuse_1, [0])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.FACE, [9])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.EDGE, [30])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.VERTEX, [60])
model.testResultsVolumes(ExtrusionFuse_1, [43388.789797644698410294950008392])

model.end()

assert(model.checkPythonDump())
