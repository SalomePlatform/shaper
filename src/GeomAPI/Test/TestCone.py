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

from GeomAPI import *
from SketchAPI import *

from salome.shaper import model

import math

TOLERANCE = 1.e-7

def assertCircle(theEdge, theCenter, theRadius):
    assert(theEdge.isCircle())
    aCircle = theEdge.circle()
    assert(aCircle.center().distance(theCenter) < TOLERANCE), "({}, {}, {}) != expected ({}, {}, {})".format(aCircle.center().x(), aCircle.center().y(), aCircle.center().z(), theCenter.x(), theCenter.y(), theCenter.z())
    assert(math.fabs(aCircle.radius() - theRadius) < TOLERANCE), "Radius {} != {}".format(aCircle.radius(), theRadius)

def checkCircleEdge(theDocument, theEdgeName, theCenter, theRadius):
    anEdge = model.addEdge(theDocument, [model.selection("EDGE", theEdgeName)])
    aShape = anEdge.result().resultSubShapePair()[0].shape()
    assert(aShape.isEdge())
    assert(aShape.edge().isClosed())
    assertCircle(aShape.edge(), theCenter, theRadius)
    theDocument.removeFeature(anEdge.feature())

def checkCircleFace(theDocument, theFaceName, theCenter, theRadius):
    aFaceFeature = model.addFace(theDocument, [model.selection("FACE", theFaceName)])
    aShape = aFaceFeature.result().resultSubShapePair()[0].shape()
    assert(aShape.isFace())
    aFace = aShape.face();
    aSubs = aFace.subShapes(GeomAPI.GeomAPI_Shape.EDGE)
    assert(aSubs.size() == 1)
    assertCircle(aSubs[0].edge(), theCenter, theRadius)
    theDocument.removeFeature(aFaceFeature.feature())

def assertEllipse(theEdge, theFirstFocus, theSecondFocus, theMajorRadius, theMinorRadius, theNormal = None):
    assert(theEdge.isEllipse())
    anEllipse = theEdge.ellipse()
    assert(anEllipse.firstFocus().distance(theFirstFocus) < TOLERANCE), "({}, {}, {}) != expected ({}, {}, {})".format(anEllipse.firstFocus().x(), anEllipse.firstFocus().y(), anEllipse.firstFocus().z(), theFirstFocus.x(), theFirstFocus.y(), theFirstFocus.z())
    assert(anEllipse.secondFocus().distance(theSecondFocus) < TOLERANCE), "({}, {}, {}) != expected ({}, {}, {})".format(anEllipse.secondFocus().x(), anEllipse.secondFocus().y(), anEllipse.secondFocus().z(), theSecondFocus.x(), theSecondFocus.y(), theSecondFocus.z())
    assert(math.fabs(anEllipse.majorRadius() - theMajorRadius) < TOLERANCE), "Major radius {} != {}".format(anEllipse.majorRadius(), theMajorRadius)
    assert(math.fabs(anEllipse.minorRadius() - theMinorRadius) < TOLERANCE), "Minor radius {} != {}".format(anEllipse.minorRadius(), theMinorRadius)

    center = GeomAPI.GeomAPI_Pnt((theFirstFocus.x() + theSecondFocus.x()) * 0.5, (theFirstFocus.y() + theSecondFocus.y()) * 0.5, (theFirstFocus.z() + theSecondFocus.z()) * 0.5)
    assert(anEllipse.center().distance(center) < TOLERANCE), "({}, {}, {}) != expected ({}, {}, {})".format(anEllipse.center().x(), anEllipse.center().y(), anEllipse.center().z(), center.x(), center.y(), center.z())

    if theNormal is not None:
        assert(math.fabs(anEllipse.normal().dot(theNormal) - 1.) < TOLERANCE), "Normal ({}, {}, {}) != ({}, {}, {})".format(anEllipse.normal().x(), anEllipse.normal().y(), anEllipse.normal().z(), theNormal.x(), theNormal.y(), theNormal.z())

def checkEllipseEdge(theDocument, theEdgeName, theFirstFocus, theSecondFocus, theMajorRadius, theMinorRadius, theNormal):
    anEdge = model.addEdge(theDocument, [model.selection("EDGE", theEdgeName)])
    aShape = anEdge.result().resultSubShapePair()[0].shape()
    assert(aShape.isEdge())
    assertEllipse(aShape.edge(), theFirstFocus, theSecondFocus, theMajorRadius, theMinorRadius, theNormal)
    theDocument.removeFeature(anEdge.feature())

def checkEllipseFace(theDocument, theFaceName, theFirstFocus, theSecondFocus, theMajorRadius, theMinorRadius):
    aFaceFeature = model.addFace(theDocument, [model.selection("FACE", theFaceName)])
    aShape = aFaceFeature.result().resultSubShapePair()[0].shape()
    assert(aShape.isFace())
    aFace = aShape.face();
    aSubs = aFace.subShapes(GeomAPI.GeomAPI_Shape.EDGE)
    assert(aSubs.size() == 1)
    assertEllipse(aSubs[0].edge(), theFirstFocus, theSecondFocus, theMajorRadius, theMinorRadius)
    theDocument.removeFeature(aFaceFeature.feature())

def assertCone(theCone, theApex, theAxis, theSemiAngle, theRadius1, theRadius2, theHeight):
    assert(theCone is not None)
    assert(theCone.isSemiInfinite() == False)
    assert(theCone.isInfinite() == False)
    anApex = theCone.apex()
    anAxis = theCone.axis()
    assert(anApex.distance(theApex) < TOLERANCE), "({}, {}, {}) != expected ({}, {}, {})".format(anApex.x(), anApex.y(), anApex.z(), theApex.x(), theApex.y(), theApex.z())
    assert(anAxis.isParallel(theAxis, TOLERANCE)), "dir({}, {}, {}) is not parallel to dir({}, {}, {})".format(anAxis.x(), anAxis.y(), anAxis.z(), theAxis.x(), theAxis.y(), theAxis.z())
    assert(math.fabs(theCone.semiAngle() - theSemiAngle) < TOLERANCE), "SemiAngle {} != {}".format(theCone.semiAngle(), theSemiAngle)
    assert(math.fabs(theCone.radius1() - theRadius1) < TOLERANCE), "Radius1 {} != {}".format(theCone.radius1(), theRadius1)
    assert(math.fabs(theCone.radius2() - theRadius2) < TOLERANCE), "Radius2 {} != {}".format(theCone.radius2(), theRadius2)
    assert(math.fabs(theCone.height() - theHeight) < TOLERANCE), "Height {} != {}".format(theCone.height(), theHeight)

def checkConeFace(theDocument, theFaceName, theApex, theAxis, theSemiAngle, theRadius1, theRadius2, theHeight):
    # check conical face
    aFace = model.addFace(theDocument, [model.selection("FACE", theFaceName)])
    aShape = aFace.result().resultSubShapePair()[0].shape()
    assert(aShape.isFace())
    assertCone(aShape.face().getCone(), theApex, theAxis, theSemiAngle, theRadius1, theRadius2, theHeight)
    theDocument.removeFeature(aFace.feature())

def checkConeShell(theDocument, theFaceNames, theApex, theAxis, theSemiAngle, theRadius1, theRadius2, theHeight):
    # check conical shell
    aSelection = []
    for name in theFaceNames:
        aSelection.append(model.selection("FACE", name))
    aShell = model.addShell(theDocument, aSelection)
    aShape = aShell.result().resultSubShapePair()[0].shape()
    assert(aShape.isShell())
    assertCone(aShape.shell().getCone(), theApex, theAxis, theSemiAngle, theRadius1, theRadius2, theHeight)
    theDocument.removeFeature(aShell.feature())

def checkConeSolid(theDocument, theFaceNames, theApex, theAxis, theSemiAngle, theRadius1, theRadius2, theHeight):
    # check cone
    aSelection = []
    for name in theFaceNames:
        aSelection.append(model.selection("FACE", name))
    aSolid = model.addSolid(theDocument, aSelection)
    aShape = aSolid.result().resultSubShapePair()[0].shape()
    assert(aShape.isSolid())
    assertCone(aShape.solid().getCone(), theApex, theAxis, theSemiAngle, theRadius1, theRadius2, theHeight)
    theDocument.removeFeature(aSolid.feature())

def checkConeAll(theDocument, theFeature, theFaceName, theApex, theAxis, theSemiAngle, theRadius1, theRadius2, theHeight):
    # check solid
    aShape = theFeature.result().resultSubShapePair()[0].shape()
    assert(aShape.isSolid())
    assertCone(aShape.solid().getCone(), theApex, theAxis, theSemiAngle, theRadius1, theRadius2, theHeight)

    checkConeShell(theDocument, [theFaceName], theApex, theAxis, theSemiAngle, theRadius1, theRadius2, theHeight)
    checkConeFace(theDocument, theFaceName, theApex, theAxis, theSemiAngle, theRadius1, theRadius2, theHeight)

def checkNonCone(theFeature):
    aShape = theFeature.result().resultSubShapePair()[0].shape()
    assert(aShape.isSolid())
    assert(aShape.solid().getCone() is None)

def checkNonConeShell(theFeature):
    aShape = theFeature.result().resultSubShapePair()[0].shape()
    assert(aShape.isShell())
    assert(aShape.shell().getCone() is None)

def checkSegment(theDocument, theEdgeName, theStartPoint, theEndPoint):
    anEdgeFeature = model.addEdge(theDocument, [model.selection("EDGE", theEdgeName)])
    aShape = anEdgeFeature.result().resultSubShapePair()[0].shape()
    assert(aShape.isEdge())
    anEdge = aShape.edge()
    assert(anEdge.isLine())
    assert(anEdge.firstPoint().distance(theStartPoint) < TOLERANCE)
    assert(anEdge.lastPoint().distance(theEndPoint) < TOLERANCE)
    theDocument.removeFeature(anEdgeFeature.feature())

def checkVertex(theDocument, theVertexName, theCoordinates):
    aVertex = model.addVertex(theDocument, [model.selection("VERTEX", theVertexName)])
    aShape = aVertex.result().resultSubShapePair()[0].shape()
    assert(aShape.isVertex())
    assert(aShape.vertex().point().distance(theCoordinates) < TOLERANCE)
    theDocument.removeFeature(aVertex.feature())

def semiAngle(theRadius1, theRadius2, theHeight):
    return math.atan(math.fabs(theRadius1 - theRadius2) / theHeight)


model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
ParamR1 = model.addParameter(Part_1_doc, "R1", "50")
ParamR2 = model.addParameter(Part_1_doc, "R2", "5")
ParamH = model.addParameter(Part_1_doc, "H", "70")
ParamShift = model.addParameter(Part_1_doc, "Shift", "5")
ParamAngle = model.addParameter(Part_1_doc, "Angle", "60")
Cone_1 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "R1", "R2", "H")
model.do()

# Test 1. Check cone
aSemiAngle = semiAngle(ParamR1.value(), ParamR2.value(), ParamH.value())
anApex = GeomAPI.GeomAPI_Pnt(0, 0, ParamR1.value() / math.tan(aSemiAngle))
anAxis = GeomAPI.GeomAPI_Dir(0, 0, 1)
checkConeAll(Part_1_doc, Cone_1, "Cone_1_1/Face_1", anApex, anAxis, aSemiAngle, ParamR2.value(), ParamR1.value(), ParamH.value())
checkCircleFace(Part_1_doc, "Cone_1_1/Face_2", GeomAPI.GeomAPI_Pnt(0, 0, ParamH.value()), ParamR2.value())
checkCircleEdge(Part_1_doc, "[Cone_1_1/Face_1][Cone_1_1/Face_2]", GeomAPI.GeomAPI_Pnt(0, 0, ParamH.value()), ParamR2.value())
checkCircleFace(Part_1_doc, "Cone_1_1/Face_3", GeomAPI.GeomAPI_Pnt(0, 0, 0), ParamR1.value())
checkCircleEdge(Part_1_doc, "[Cone_1_1/Face_1][Cone_1_1/Face_3]", GeomAPI.GeomAPI_Pnt(0, 0, 0), ParamR1.value())
checkSegment(Part_1_doc, "[Cone_1_1/Face_1][weak_name_3]", GeomAPI.GeomAPI_Pnt(ParamR1.value(), 0, 0), GeomAPI.GeomAPI_Pnt(ParamR2.value(), 0, ParamH.value()))

# Test 2. Update cone radii
ParamR1.setValue(0)
model.do()
aSemiAngle = semiAngle(ParamR1.value(), ParamR2.value(), ParamH.value())
anApex.setZ(0)
checkConeAll(Part_1_doc, Cone_1, "Cone_1_1/Face_1", anApex, anAxis, aSemiAngle, ParamR1.value(), ParamR2.value(), ParamH.value())
checkCircleFace(Part_1_doc, "Cone_1_1/Face_2", GeomAPI.GeomAPI_Pnt(0, 0, ParamH.value()), ParamR2.value())
checkCircleEdge(Part_1_doc, "[Cone_1_1/Face_1][Cone_1_1/Face_2]", GeomAPI.GeomAPI_Pnt(0, 0, ParamH.value()), ParamR2.value())
checkSegment(Part_1_doc, "[Cone_1_1/Face_1][weak_name_3]", GeomAPI.GeomAPI_Pnt(0, 0, 0), GeomAPI.GeomAPI_Pnt(ParamR2.value(), 0, ParamH.value()))
checkVertex(Part_1_doc, "Cone_1_1/Vertex_2", GeomAPI.GeomAPI_Pnt(0, 0, 0))

ParamR2.setValue(50)
ParamR1.setValue(10)
model.do()
aSemiAngle = semiAngle(ParamR1.value(), ParamR2.value(), ParamH.value())
anApex.setZ(-ParamR1.value() / math.tan(aSemiAngle))
checkConeAll(Part_1_doc, Cone_1, "Cone_1_1/Face_1", anApex, anAxis, aSemiAngle, ParamR1.value(), ParamR2.value(), ParamH.value())
checkCircleFace(Part_1_doc, "Cone_1_1/Face_2", GeomAPI.GeomAPI_Pnt(0, 0, ParamH.value()), ParamR2.value())
checkCircleEdge(Part_1_doc, "[Cone_1_1/Face_1][Cone_1_1/Face_2]", GeomAPI.GeomAPI_Pnt(0, 0, ParamH.value()), ParamR2.value())
checkCircleFace(Part_1_doc, "Cone_1_1/Face_3", GeomAPI.GeomAPI_Pnt(0, 0, 0), ParamR1.value())
checkCircleEdge(Part_1_doc, "[Cone_1_1/Face_1][Cone_1_1/Face_3]", GeomAPI.GeomAPI_Pnt(0, 0, 0), ParamR1.value())
checkSegment(Part_1_doc, "[Cone_1_1/Face_1][weak_name_3]", GeomAPI.GeomAPI_Pnt(ParamR1.value(), 0, 0), GeomAPI.GeomAPI_Pnt(ParamR2.value(), 0, ParamH.value()))

# Test 3. Translate cone
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Cone_1_1")], model.selection("EDGE", "PartSet/OX"), "Shift")
anApex.setX(anApex.x() + ParamShift.value())
checkConeAll(Part_1_doc, Translation_1, "Translation_1_1/MF:Translated&Cone_1_1/Face_1", anApex, anAxis, aSemiAngle, ParamR1.value(), ParamR2.value(), ParamH.value())
checkCircleFace(Part_1_doc, "Translation_1_1/MF:Translated&Cone_1_1/Face_2", GeomAPI.GeomAPI_Pnt(ParamShift.value(), 0, ParamH.value()), ParamR2.value())
checkCircleEdge(Part_1_doc, "[Translation_1_1/MF:Translated&Cone_1_1/Face_1][Translation_1_1/MF:Translated&Cone_1_1/Face_2]", GeomAPI.GeomAPI_Pnt(ParamShift.value(), 0, ParamH.value()), ParamR2.value())
checkCircleFace(Part_1_doc, "Translation_1_1/MF:Translated&Cone_1_1/Face_3", GeomAPI.GeomAPI_Pnt(ParamShift.value(), 0, 0), ParamR1.value())
checkCircleEdge(Part_1_doc, "[Translation_1_1/MF:Translated&Cone_1_1/Face_1][Translation_1_1/MF:Translated&Cone_1_1/Face_3]", GeomAPI.GeomAPI_Pnt(ParamShift.value(), 0, 0), ParamR1.value())
checkSegment(Part_1_doc, "[Translation_1_1/MF:Translated&Cone_1_1/Face_1][weak_name_3]", GeomAPI.GeomAPI_Pnt(ParamR1.value() + ParamShift.value(), 0, 0), GeomAPI.GeomAPI_Pnt(ParamR2.value() + ParamShift.value(), 0, ParamH.value()))

# Test 4. Rotate cone
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Translation_1_1")], model.selection("EDGE", "PartSet/OY"), "Angle")
anAngle = ParamAngle.value() * math.pi / 180.0
anAxis = GeomAPI.GeomAPI_Dir(math.sin(anAngle), 0, math.cos(anAngle))
x, z = anApex.x(), anApex.z()
anApex.setX(x * math.cos(anAngle) + z * math.sin(anAngle))
anApex.setZ(-x * math.sin(anAngle) + z * math.cos(anAngle))
checkConeAll(Part_1_doc, Rotation_1, "Rotation_1_1/MF:Rotated&Cone_1_1/Face_1", anApex, anAxis, aSemiAngle, ParamR1.value(), ParamR2.value(), ParamH.value())
aCenter = GeomAPI.GeomAPI_Pnt(ParamShift.value() * math.cos(anAngle) + ParamH.value() * math.sin(anAngle), 0, -ParamShift.value() * math.sin(anAngle) + ParamH.value() * math.cos(anAngle))
checkCircleFace(Part_1_doc, "Rotation_1_1/MF:Rotated&Cone_1_1/Face_2", aCenter, ParamR2.value())
checkCircleEdge(Part_1_doc, "[Rotation_1_1/MF:Rotated&Cone_1_1/Face_1][Rotation_1_1/MF:Rotated&Cone_1_1/Face_2]", aCenter, ParamR2.value())
aCenter = GeomAPI.GeomAPI_Pnt(ParamShift.value() * math.cos(anAngle), 0, -ParamShift.value() * math.sin(anAngle))
checkCircleFace(Part_1_doc, "Rotation_1_1/MF:Rotated&Cone_1_1/Face_3", aCenter, ParamR1.value())
checkCircleEdge(Part_1_doc, "[Rotation_1_1/MF:Rotated&Cone_1_1/Face_1][Rotation_1_1/MF:Rotated&Cone_1_1/Face_3]", aCenter, ParamR1.value())

# Test 5. Split cone by plane and check conical shell and elliptic face
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/YOZ"), 20, False)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Rotation_1_1"), model.selection("FACE", "Plane_1")])
checkConeShell(Part_1_doc, ["Partition_1_1_1/Modified_Face&Cone_1_1/Face_1", "Partition_1_1_2/Modified_Face&Cone_1_1/Face_1"], anApex, anAxis, aSemiAngle, ParamR1.value(), ParamR2.value(), ParamH.value())

aFirstFocus = GeomAPI.GeomAPI_Pnt(20, 0, 31.062397266842858)
aSecondFocus = GeomAPI.GeomAPI_Pnt(20, 0, -1.0935246846933797)
aMajorRadius = 27.91915871311068
aMinorRadius = 22.824955511666207
aNormal = GeomAPI.GeomAPI_Dir(1, 0, 0)
checkEllipseFace(Part_1_doc, "_weak_name_1_Partition_1_1_2", aFirstFocus, aSecondFocus, aMajorRadius, aMinorRadius)
checkEllipseEdge(Part_1_doc, "Partition_1_1_1/Generated_Edge&Cone_1_1/Face_1", aFirstFocus, aSecondFocus, aMajorRadius, aMinorRadius, aNormal)

# Test 6. Compose a conical solid
Solid_1_objects = ["Rotation_1_1/MF:Rotated&Cone_1_1/Face_3",
                   "Partition_1_1_1/Modified_Face&Cone_1_1/Face_1",
                   "Partition_1_1_2/Modified_Face&Cone_1_1/Face_1",
                   "Rotation_1_1/MF:Rotated&Cone_1_1/Face_2"]
checkConeSolid(Part_1_doc, Solid_1_objects, anApex, anAxis, aSemiAngle, ParamR1.value(), ParamR2.value(), ParamH.value())

# Test 7. Check non-cone
Cone_2 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10, 5, 10)
Cone_3 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10, 20)
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Cone_2_1"), model.selection("SOLID", "Cone_3_1")], True)

model.do()
Solid_2_objects = [model.selection("FACE", "Fuse_1_1/Modified_Face&Cone_2_1/Face_3&Cone_3_1/Face_3"),
                   model.selection("FACE", "Fuse_1_1/Modified_Face&Cone_2_1/Face_1"),
                   model.selection("FACE", "Fuse_1_1/Modified_Face&Cone_3_1/Face_1"),
                   model.selection("FACE", "Cone_3_1/Face_2")]
Solid_2 = model.addSolid(Part_1_doc, Solid_2_objects)
checkNonCone(Solid_2)
model.end()

# in order to use study objects once again, undo Test7 actions
model.undo()

# Test 8. Check non-conical shell
model.begin()
Shell_1_objects = [model.selection("FACE", "Rotation_1_1/MF:Rotated&Cone_1_1/Face_3"),
                   model.selection("FACE", "Partition_1_1_1/Modified_Face&Cone_1_1/Face_1"),
                   model.selection("FACE", "Partition_1_1_2/Modified_Face&Cone_1_1/Face_1"),
                   model.selection("FACE", "Rotation_1_1/MF:Rotated&Cone_1_1/Face_2")]
Shell_1 = model.addShell(Part_1_doc, Shell_1_objects)
checkNonConeShell(Shell_1)

Shell_2 = model.addShell(Part_1_doc, [model.selection("FACE", "Fuse_1_1/Modified_Face&Cone_2_1/Face_1"), model.selection("FACE", "Fuse_1_1/Modified_Face&Cone_3_1/Face_1")])
checkNonConeShell(Shell_2)
model.end()

# in order to use study objects once again, undo Test8 actions
model.undo()

# Test 9. Check error on conversion to wrong type of curve
model.begin()
anEdge = model.addEdge(Part_1_doc, [model.selection("EDGE", "[Partition_1_1_2/Modified_Face&Cone_1_1/Face_1][Rotation_1_1/MF:Rotated&Cone_1_1/Face_2]")])
aShape = anEdge.result().resultSubShapePair()[0].shape()
assert(aShape.isEdge())
assert(aShape.edge().ellipse() is None)
assert(aShape.edge().line() is None)
model.end()

model.undo()

model.begin()

anEdge = model.addEdge(Part_1_doc, [model.selection("EDGE", "[Partition_1_1_2/Modified_Face&Cone_1_1/Face_1][weak_name_2]")])
aShape = anEdge.result().resultSubShapePair()[0].shape()
assert(aShape.isEdge())
assert(aShape.edge().circle() is None)
assert(aShape.isSelfIntersected() == False)

# Test 10. Test cone constructors
apex = GeomAPI_Pnt(10, 0, 0)
dir = GeomAPI_Dir(1, 0, 0)
semiAngle = math.pi / 4
cone = GeomAPI_Cone(apex, dir, semiAngle)
assert(cone.location().distance(apex) < TOLERANCE)

radius = 5
cone = GeomAPI_Cone(apex, dir, semiAngle, radius)
assert(cone.location().distance(apex) < TOLERANCE)

model.end()
