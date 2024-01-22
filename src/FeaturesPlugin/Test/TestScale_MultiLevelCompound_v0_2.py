# Copyright (C) 2020-2024  CEA, EDF
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

from salome.shaper import model

from GeomAPI import *
from SketchAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchEllipse_1 = Sketch_1.addEllipse(11.18033988749894, -50, 22.36067977499789, -50, 10)
[SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchLine_1, SketchLine_2] = SketchEllipse_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 30)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), 20)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_3 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_3).coordinates(), SketchLine_3.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_3).startPoint(), SketchAPI_Point(SketchPoint_3).coordinates(), 50, True)
SketchCircle_1 = Sketch_1.addCircle(41.18033988749897, -50, 5)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_1.result())
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchEllipse_1.majorAxisPositive(), SketchCircle_1.center(), 15, True)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 5)
SketchMultiRotation_1 = Sketch_1.addRotation([SketchEllipse_1.result(), SketchCircle_1.results()[1]], SketchAPI_Point(SketchPoint_3).coordinates(), 360, 3, True)
[SketchEllipse_2, SketchEllipse_3, SketchCircle_2, SketchCircle_3] = SketchMultiRotation_1.rotated()
model.do()
Sketch_1.changeFacesOrder([[SketchEllipse_1.result(), SketchEllipse_1.result(), SketchEllipse_2.result(), SketchEllipse_3.result()],
                           [SketchCircle_1.results()[1]],
                           [SketchEllipse_2.result(), SketchEllipse_2.result(), SketchEllipse_3.result(), SketchEllipse_1.result()],
                           [SketchEllipse_1.result(), SketchEllipse_3.result(), SketchEllipse_2.result()],
                           [SketchEllipse_3.result(), SketchEllipse_2.result(), SketchEllipse_1.result()],
                           [SketchEllipse_1.result(), SketchEllipse_2.result(), SketchEllipse_3.result()],
                           [SketchEllipse_1.result(), SketchEllipse_3.result(), SketchEllipse_2.result()],
                           [SketchEllipse_2.result(), SketchEllipse_3.result(), SketchEllipse_3.result(), SketchEllipse_1.result()],
                           [SketchCircle_2.results()[1]],
                           [SketchCircle_3.results()[1]]
                          ])
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("SOLID", "Extrusion_1_2"), model.selection("SOLID", "Extrusion_1_3")])
Compound_2_objects = [model.selection("COMPSOLID", "Extrusion_1_1"), model.selection("COMPOUND", "Compound_1_1"), model.selection("SOLID", "Extrusion_1_4")]
Compound_2 = model.addCompound(Part_1_doc, Compound_2_objects)
model.end()

SCALE_X = 1
SCALE_Y = 2
SCALE_Z = 3
TOLERANCE = 1.e-3


model.begin()
Scale_1 = model.addScale(Part_1_doc, [model.selection("SOLID", "Compound_2_1_1_5")], model.selection("VERTEX", "PartSet/Origin"), SCALE_X, SCALE_Y, SCALE_Z)
model.testNbResults(Scale_1, 1)
model.testNbSubResults(Scale_1, [0])
model.testNbSubShapes(Scale_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_1, GeomAPI_Shape.FACE, [5])
model.testNbSubShapes(Scale_1, GeomAPI_Shape.EDGE, [18])
model.testNbSubShapes(Scale_1, GeomAPI_Shape.VERTEX, [36])
model.testResultsVolumes(Scale_1, [542.746463956 * SCALE_X * SCALE_Y * SCALE_Z])

refPoint = Extrusion_1.results()[0].subResult(4).resultSubShapePair()[0].shape().middlePoint()
refPoint = GeomAPI_Pnt(refPoint.x() * SCALE_X, refPoint.y() * SCALE_Y, refPoint.z() * SCALE_Z)
midPoint = Scale_1.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)


Recover_1 = model.addRecover(Part_1_doc, Scale_1, [Compound_2.result()], True)
Scale_2 = model.addScale(Part_1_doc, [model.selection("SOLID", "Recover_1_1_2_1")], model.selection("VERTEX", "PartSet/Origin"), SCALE_X, SCALE_Y, SCALE_Z)
model.testNbResults(Scale_2, 1)
model.testNbSubResults(Scale_2, [0])
model.testNbSubShapes(Scale_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_2, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(Scale_2, GeomAPI_Shape.EDGE, [6])
model.testNbSubShapes(Scale_2, GeomAPI_Shape.VERTEX, [12])
model.testResultsVolumes(Scale_2, [785.3997 * SCALE_X * SCALE_Y * SCALE_Z])

refPoint = Recover_1.result().subResult(1).subResult(0).resultSubShapePair()[0].shape().middlePoint()
refPoint = GeomAPI_Pnt(refPoint.x() * SCALE_X, refPoint.y() * SCALE_Y, refPoint.z() * SCALE_Z)
midPoint = Scale_2.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)


Recover_2 = model.addRecover(Part_1_doc, Scale_2, [Recover_1.result()], True)
Scale_3 = model.addScale(Part_1_doc, [model.selection("SOLID", "Recover_2_1_3")], model.selection("VERTEX", "PartSet/Origin"), SCALE_X, SCALE_Y, SCALE_Z)
model.testNbResults(Scale_3, 1)
model.testNbSubResults(Scale_3, [0])
model.testNbSubShapes(Scale_3, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_3, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(Scale_3, GeomAPI_Shape.EDGE, [6])
model.testNbSubShapes(Scale_3, GeomAPI_Shape.VERTEX, [12])
model.testResultsVolumes(Scale_3, [785.3997 * SCALE_X * SCALE_Y * SCALE_Z])

refPoint = Recover_2.result().subResult(2).resultSubShapePair()[0].shape().middlePoint()
refPoint = GeomAPI_Pnt(refPoint.x() * SCALE_X, refPoint.y() * SCALE_Y, refPoint.z() * SCALE_Z)
midPoint = Scale_3.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)


Recover_3 = model.addRecover(Part_1_doc, Scale_3, [Recover_2.result()], True)
Scale_4 = model.addScale(Part_1_doc, [model.selection("SOLID", "Recover_3_1_1_1"), model.selection("SOLID", "Recover_3_1_2_2"), model.selection("SOLID", "Recover_3_1_3")], model.selection("VERTEX", "PartSet/Origin"), 1 / SCALE_X, 1 / SCALE_Y, 1 / SCALE_Z)
model.testNbResults(Scale_4, 3)
model.testNbSubResults(Scale_4, [0, 0, 0])
model.testNbSubShapes(Scale_4, GeomAPI_Shape.SOLID, [1, 1, 1])
model.testNbSubShapes(Scale_4, GeomAPI_Shape.FACE, [6, 3, 3])
model.testNbSubShapes(Scale_4, GeomAPI_Shape.EDGE, [24, 6, 6])
model.testNbSubShapes(Scale_4, GeomAPI_Shape.VERTEX, [48, 12, 12])
model.testResultsVolumes(Scale_4, [3444.39814733 / (SCALE_X * SCALE_Y * SCALE_Z), 785.3997 / (SCALE_X * SCALE_Y * SCALE_Z), 785.3997 / (SCALE_X * SCALE_Y * SCALE_Z)])

REFERENCE = [Recover_3.result().subResult(0).subResult(0).resultSubShapePair()[0].shape().middlePoint(),
             Recover_3.result().subResult(1).subResult(1).resultSubShapePair()[0].shape().middlePoint(),
             Recover_3.result().subResult(2).resultSubShapePair()[0].shape().middlePoint()]
for res, ref in zip(Scale_4.results(), REFERENCE):
    ref = GeomAPI_Pnt(ref.x() / SCALE_X, ref.y() / SCALE_Y, ref.z() / SCALE_Z)
    midPoint = res.resultSubShapePair()[0].shape().middlePoint()
    assert(midPoint.distance(ref) < TOLERANCE)

model.end()

assert(model.checkPythonDump())
