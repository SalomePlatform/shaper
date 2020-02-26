# Copyright (C) 2020  CEA/DEN, EDF R&D
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
from GeomAlgoAPI import *
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

DISTANCE = 20
COPIES = 3
TOLERANCE = 1.e-7


model.begin()
MultiTranslation_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Compound_2_1_1_5")], model.selection("EDGE", "PartSet/OZ"), DISTANCE, COPIES)
model.testNbResults(MultiTranslation_1, 1)
model.testNbSubResults(MultiTranslation_1, [COPIES])
model.testNbSubShapes(MultiTranslation_1, GeomAPI_Shape.SOLID, [1 * COPIES])
model.testNbSubShapes(MultiTranslation_1, GeomAPI_Shape.FACE, [5 * COPIES])
model.testNbSubShapes(MultiTranslation_1, GeomAPI_Shape.EDGE, [18 * COPIES])
model.testNbSubShapes(MultiTranslation_1, GeomAPI_Shape.VERTEX, [36 * COPIES])
model.testResultsVolumes(MultiTranslation_1, [542.746463956 * COPIES])

comp = GeomAlgoAPI_CompoundBuilder.compound([Extrusion_1.results()[0].subResult(4).resultSubShapePair()[0].shape()])
refPoint = comp.middlePoint()
refPoint.setZ(refPoint.z() + DISTANCE / 2 * (COPIES - 1))
midPoint = MultiTranslation_1.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)


Recover_1 = model.addRecover(Part_1_doc, MultiTranslation_1, [Compound_2.result()], True)
MultiTranslation_2 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Recover_1_1_2_1")], model.selection("EDGE", "PartSet/OZ"), DISTANCE, COPIES)
model.testNbResults(MultiTranslation_2, 1)
model.testNbSubResults(MultiTranslation_2, [COPIES])
model.testNbSubShapes(MultiTranslation_2, GeomAPI_Shape.SOLID, [1 * COPIES])
model.testNbSubShapes(MultiTranslation_2, GeomAPI_Shape.FACE, [3 * COPIES])
model.testNbSubShapes(MultiTranslation_2, GeomAPI_Shape.EDGE, [6 * COPIES])
model.testNbSubShapes(MultiTranslation_2, GeomAPI_Shape.VERTEX, [12 * COPIES])
model.testResultsVolumes(MultiTranslation_2, [785.39816339745 * COPIES])

refPoint = Recover_1.result().subResult(1).subResult(0).resultSubShapePair()[0].shape().middlePoint()
refPoint.setZ(refPoint.z() + DISTANCE / 2 * (COPIES - 1))
midPoint = MultiTranslation_2.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)


Recover_2 = model.addRecover(Part_1_doc, MultiTranslation_2, [Recover_1.result()], True)
MultiTranslation_3 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Recover_2_1_3")], model.selection("EDGE", "PartSet/OZ"), DISTANCE, COPIES)
model.testNbResults(MultiTranslation_3, 1)
model.testNbSubResults(MultiTranslation_3, [COPIES])
model.testNbSubShapes(MultiTranslation_3, GeomAPI_Shape.SOLID, [1 * COPIES])
model.testNbSubShapes(MultiTranslation_3, GeomAPI_Shape.FACE, [3 * COPIES])
model.testNbSubShapes(MultiTranslation_3, GeomAPI_Shape.EDGE, [6 * COPIES])
model.testNbSubShapes(MultiTranslation_3, GeomAPI_Shape.VERTEX, [12 * COPIES])
model.testResultsVolumes(MultiTranslation_3, [785.39816339745 * COPIES])

refPoint = Recover_2.result().subResult(2).resultSubShapePair()[0].shape().middlePoint()
refPoint.setZ(refPoint.z() + DISTANCE / 2 * (COPIES - 1))
midPoint = MultiTranslation_3.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)


Recover_3 = model.addRecover(Part_1_doc, MultiTranslation_3, [Recover_2.result()], True)
MultiTranslation_4 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Recover_3_1_1_1"), model.selection("SOLID", "Recover_3_1_2_2"), model.selection("SOLID", "Recover_3_1_3")], model.selection("EDGE", "PartSet/OZ"), DISTANCE, COPIES)
model.testNbResults(MultiTranslation_4, 3)
model.testNbSubResults(MultiTranslation_4, [COPIES, COPIES, COPIES])
model.testNbSubShapes(MultiTranslation_4, GeomAPI_Shape.SOLID, [1 * COPIES, 1 * COPIES, 1 * COPIES])
model.testNbSubShapes(MultiTranslation_4, GeomAPI_Shape.FACE, [6 * COPIES, 3 * COPIES, 3 * COPIES])
model.testNbSubShapes(MultiTranslation_4, GeomAPI_Shape.EDGE, [24 * COPIES, 6 * COPIES, 6 * COPIES])
model.testNbSubShapes(MultiTranslation_4, GeomAPI_Shape.VERTEX, [48 * COPIES, 12 * COPIES, 12 * COPIES])
model.testResultsVolumes(MultiTranslation_4, [3444.394198615 * COPIES, 785.39816339745 * COPIES, 785.39816339745 * COPIES])

comp = GeomAlgoAPI_CompoundBuilder.compound([Recover_3.result().subResult(0).subResult(0).resultSubShapePair()[0].shape()])
REFERENCE = [comp.middlePoint(),
             Recover_3.result().subResult(1).subResult(1).resultSubShapePair()[0].shape().middlePoint(),
             Recover_3.result().subResult(2).resultSubShapePair()[0].shape().middlePoint()]
for res, ref in zip(MultiTranslation_4.results(), REFERENCE):
    ref.setZ(ref.z() + DISTANCE / 2 * (COPIES - 1))
    midPoint = res.resultSubShapePair()[0].shape().middlePoint()
    assert(midPoint.distance(ref) < TOLERANCE)

model.end()

assert(model.checkPythonDump())
