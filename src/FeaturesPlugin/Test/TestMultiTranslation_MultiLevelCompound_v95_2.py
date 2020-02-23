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

TOLERANCE = 1.e-7
DISTANCE_1 = 100
COPIES_1 = 2
DISTANCE_2 = 50
COPIES_2 = 3
COPIES = COPIES_1 * COPIES_2


def subsRefPoints(theResult):
    res = []
    for ind in range(0, theResult.numberOfSubs()):
        res.append( theResult.subResult(ind).resultSubShapePair()[0].shape().middlePoint())
    return res

REFERENCE = subsRefPoints(Compound_2.result())

def assertResult(theFeature, theNbSolids, theNbFaces, theNbEdges, theNbVertices, theVolume):
    model.testNbResults(theFeature, 1)
    model.testNbSubResults(theFeature, [3])
    model.testNbSubShapes(theFeature, GeomAPI_Shape.SOLID, [theNbSolids])
    model.testNbSubShapes(theFeature, GeomAPI_Shape.FACE, [theNbFaces])
    model.testNbSubShapes(theFeature, GeomAPI_Shape.EDGE, [theNbEdges])
    model.testNbSubShapes(theFeature, GeomAPI_Shape.VERTEX, [theNbVertices])
    model.testResultsVolumes(theFeature, [theVolume])

    for ind in range(0, theFeature.result().numberOfSubs()):
        ref = REFERENCE[ind]
        midPoint = theFeature.result().subResult(ind).resultSubShapePair()[0].shape().middlePoint()
        assert(midPoint.distance(ref) < TOLERANCE), "Sub-result {}; actual ({}, {}, {}) != expected ({}, {}, {})".format(ind, midPoint.x(), midPoint.y(), midPoint.z(), ref.x(), ref.y(), ref.z())


model.begin()
MultiTranslation_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Compound_2_1_1_5")], model.selection("EDGE", "PartSet/OY"), DISTANCE_1, COPIES_1, model.selection("EDGE", "PartSet/OZ"), DISTANCE_2, COPIES_2, keepSubResults = True)
model.end()
# selection of a compsolid part is prohibited
assert(MultiTranslation_1.feature().error() != "")

model.begin()
MultiTranslation_1.setMainObjects([model.selection("COMPSOLID", "Compound_2_1_1")])
REFERENCE[0].setY(REFERENCE[0].y() + DISTANCE_1 / 2 * (COPIES_1 - 1))
REFERENCE[0].setZ(REFERENCE[0].z() + DISTANCE_2 / 2 * (COPIES_2 - 1))
assertResult(MultiTranslation_1, 45, 237, 882, 1764, 74139.0037)

MultiTranslation_2 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "LinearCopy_1_1_2_1")], model.selection("EDGE", "PartSet/OY"), DISTANCE_1, COPIES_1, model.selection("EDGE", "PartSet/OZ"), DISTANCE_2, COPIES_2, keepSubResults = True)
ref1Ycopy = REFERENCE[1].y()
REFERENCE[1].setY(0)
REFERENCE[1].setZ(REFERENCE[1].z() + DISTANCE_2 / 2 * (COPIES_2 - 1))
assertResult(MultiTranslation_2, 50, 252, 912, 1824, 78065.9945)

MultiTranslation_3 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "LinearCopy_2_1_3")], model.selection("EDGE", "PartSet/OY"), DISTANCE_1, COPIES_1, model.selection("EDGE", "PartSet/OZ"), DISTANCE_2, COPIES_2, keepSubResults = True)
REFERENCE[2].setY(REFERENCE[2].y() + DISTANCE_1 / 2 * (COPIES_1 - 1))
REFERENCE[2].setZ(REFERENCE[2].z() + DISTANCE_2 / 2 * (COPIES_2 - 1))
assertResult(MultiTranslation_3, 55, 267, 942, 1884, 81992.9853)

MultiTranslation_4 = model.addMultiTranslation(Part_1_doc, [model.selection("COMPSOLID", "LinearCopy_3_1_1_5"), model.selection("SOLID", "LinearCopy_3_1_2_2"), model.selection("COMPOUND", "LinearCopy_3_1_3")], model.selection("EDGE", "PartSet/OX"), -DISTANCE_2, COPIES_2, model.selection("EDGE", "PartSet/OY"), -DISTANCE_1, COPIES_1, keepSubResults = True)
REFERENCE[0].setX(REFERENCE[0].x() - DISTANCE_2 / 2 * (COPIES_2 - 1))
REFERENCE[0].setY(REFERENCE[0].y() - DISTANCE_1 / 2 * (COPIES_1 - 1))
REFERENCE[1].setX(REFERENCE[1].x() - DISTANCE_2 / 2 * (COPIES_2 - 1))
REFERENCE[1].setY(ref1Ycopy)
REFERENCE[2].setX(REFERENCE[2].x() - DISTANCE_2 / 2 * (COPIES_2 - 1))
REFERENCE[2].setY(REFERENCE[2].y() - DISTANCE_1 / 2 * (COPIES_1 - 1))
assertResult(MultiTranslation_4, 125, 562, 1872, 3744, 169300.9287)

model.end()

assert(model.checkPythonDump())
