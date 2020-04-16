# Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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

def assertCylinder(theCylinder, theLocation, theAxis, theRadius, theHeight):
    assert(theCylinder is not None)
    assert(theCylinder.isInfinite() == False)
    aLoc = theCylinder.location()
    aDir = theCylinder.axis()
    assert(aLoc.distance(theLocation) < TOLERANCE), "({}, {}, {}) != expected ({}, {}, {})".format(aLoc.x(), aLoc.y(), aLoc.z(), theLocation.x(), theLocation.y(), theLocation.z())
    assert(aDir.isParallel(theAxis, TOLERANCE)), "dir({}, {}, {}) is not parallel to dir({}, {}, {})".format(aDir.x(), aDir.y(), aDir.z(), theAxis.x(), theAxis.y(), theAxis.z())
    assert(math.fabs(theCylinder.radius() - theRadius) < TOLERANCE), "Radius {} != {}".format(theCylinder.radius(), theRadius)
    assert(math.fabs(theCylinder.height() - theHeight) < TOLERANCE), "Height {} != {}".format(theCylinder.height(), theHeight)

def checkCylinderFace(theDocument, theFaceName, theLocation, theAxis, theRadius, theHeight):
    aFace = model.addFace(theDocument, [model.selection("FACE", theFaceName)])
    aShape = aFace.result().resultSubShapePair()[0].shape()
    assert(aShape.isFace())
    assertCylinder(aShape.face().getCylinder(), theLocation, theAxis, theRadius, theHeight)
    theDocument.removeFeature(aFace.feature())

def checkCylinderShell(theDocument, theFaceNames, theLocation, theAxis, theRadius, theHeight):
    aSelection = []
    for name in theFaceNames:
        aSelection.append(model.selection("FACE", name))
    aShell = model.addShell(theDocument, aSelection)
    aShape = aShell.result().resultSubShapePair()[0].shape()
    assert(aShape.isShell())
    assertCylinder(aShape.shell().getCylinder(), theLocation, theAxis, theRadius, theHeight)
    theDocument.removeFeature(aShell.feature())

def checkCylinderSolid(theDocument, theFaceNames, theLocation, theAxis, theRadius, theHeight):
    aSelection = []
    for name in theFaceNames:
        aSelection.append(model.selection("FACE", name))
    aSolid = model.addSolid(theDocument, aSelection)
    aShape = aSolid.result().resultSubShapePair()[0].shape()
    assert(aShape.isSolid())
    assertCylinder(aShape.solid().getCylinder(), theLocation, theAxis, theRadius, theHeight)
    theDocument.removeFeature(aSolid.feature())

def checkCylinderAll(theDocument, theFeature, theFaceName, theLocation, theAxis, theRadius, theHeight):
    aShape = theFeature.result().resultSubShapePair()[0].shape()
    assert(aShape.isSolid())
    assertCylinder(aShape.solid().getCylinder(), theLocation, theAxis, theRadius, theHeight)

    checkCylinderShell(theDocument, [theFaceName], theLocation, theAxis, theRadius, theHeight)
    checkCylinderFace(theDocument, theFaceName, theLocation, theAxis, theRadius, theHeight)

def checkNonCylinder(theFeature):
    aShape = theFeature.result().resultSubShapePair()[0].shape()
    assert(aShape.isSolid())
    assert(aShape.solid().getCylinder() is None)

def checkNonCylindricalShell(theFeature):
    aShape = theFeature.result().resultSubShapePair()[0].shape()
    assert(aShape.isShell())
    assert(aShape.shell().getCylinder() is None)


model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
ParamH = model.addParameter(Part_1_doc, "H", "10")
ParamR = model.addParameter(Part_1_doc, "R", "10")
ParamAngle = model.addParameter(Part_1_doc, "Angle", "30")
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "2*R", "H")
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Cylinder_1_1/Face_2"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "[Cylinder_1_1/Face_1][Cylinder_1_1/Face_2]__cc"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 10)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], "R")
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f")], model.selection(), "H", 0)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchLine_1 = Sketch_2.addLine(5, 20, 0, 20)
SketchLine_2 = Sketch_2.addLine(0, 20, 0, 30)
SketchLine_3 = Sketch_2.addLine(0, 30, 5, 30)
SketchLine_4 = Sketch_2.addLine(5, 30, 5, 20)
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_2.setLength(SketchLine_1.result(), "R/2")
SketchConstraintLength_2 = Sketch_2.setLength(SketchLine_2.result(), "H")
SketchIntersectionPoint_1 = Sketch_2.addIntersectionPoint(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/To_Face]"), False)
[SketchPoint_2, SketchPoint_3] = SketchIntersectionPoint_1.intersectionPoints()
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_1.result())
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_5 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_5.result())
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection("EDGE", "PartSet/OZ"), 270, 0)
model.do()

# Test 1. Check cylinders
aLoc1 = GeomAPI.GeomAPI_Pnt(0, 0, 0)
aLoc2 = GeomAPI.GeomAPI_Pnt(0, 0, 2 * ParamH.value())
aLoc3 = GeomAPI.GeomAPI_Pnt(0, 0, 3 * ParamH.value())
anAxis = GeomAPI.GeomAPI_Dir(0, 0, 1)
checkCylinderAll(Part_1_doc, Cylinder_1, "Cylinder_1_1/Face_1", aLoc1, anAxis, 2 * ParamR.value(), ParamH.value())
checkCylinderAll(Part_1_doc, Extrusion_1, "Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2", aLoc2, anAxis, ParamR.value(), ParamH.value())
checkNonCylinder(Revolution_1)
checkCylinderShell(Part_1_doc, ["Revolution_1_1/Generated_Face&Sketch_2/SketchLine_4"], aLoc3, anAxis, 0.5 * ParamR.value(), ParamH.value())
checkCylinderFace(Part_1_doc, "Revolution_1_1/Generated_Face&Sketch_2/SketchLine_4", aLoc3, anAxis, 0.5 * ParamR.value(), ParamH.value())

# Test 2. Rotate cylinders
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")], model.selection("EDGE", "PartSet/OX"), "Angle")
Rotation_2 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], model.selection("EDGE", "PartSet/OX"), "Angle")
Rotation_3 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Revolution_1_1")], model.selection("EDGE", "PartSet/OX"), "Angle")

anAngle = ParamAngle.value() * math.pi / 180.0
aCosAngle = math.cos(anAngle)
aSinAngle = math.sin(anAngle)
anAxis = GeomAPI.GeomAPI_Dir(0, anAxis.y() * aCosAngle - anAxis.z() * aSinAngle, anAxis.y() * aSinAngle + anAxis.z() * aCosAngle)
aLoc1 = GeomAPI.GeomAPI_Pnt(0, aLoc1.y() * aCosAngle - aLoc1.z() * aSinAngle, aLoc1.y() * aSinAngle + aLoc1.z() * aCosAngle)
aLoc2 = GeomAPI.GeomAPI_Pnt(0, aLoc2.y() * aCosAngle - aLoc2.z() * aSinAngle, aLoc2.y() * aSinAngle + aLoc2.z() * aCosAngle)
aLoc3 = GeomAPI.GeomAPI_Pnt(0, aLoc3.y() * aCosAngle - aLoc3.z() * aSinAngle, aLoc3.y() * aSinAngle + aLoc3.z() * aCosAngle)
checkCylinderAll(Part_1_doc, Rotation_1, "Rotation_1_1/MF:Rotated&Cylinder_1_1/Face_1", aLoc1, anAxis, 2 * ParamR.value(), ParamH.value())
checkCylinderAll(Part_1_doc, Rotation_2, "Rotation_2_1/MF:Rotated&Sketch_1/SketchCircle_1_2", aLoc2, anAxis, ParamR.value(), ParamH.value())
checkNonCylinder(Rotation_3)
checkCylinderShell(Part_1_doc, ["Rotation_3_1/MF:Rotated&Sketch_2/SketchLine_4"], aLoc3, anAxis, 0.5 * ParamR.value(), ParamH.value())
checkCylinderFace(Part_1_doc, "Rotation_3_1/MF:Rotated&Sketch_2/SketchLine_4", aLoc3, anAxis, 0.5 * ParamR.value(), ParamH.value())

# Test 3. Split cylinder and compose a shell
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), "2.2*H", False)
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOZ"), "H", False)
Partition_1_objects = [model.selection("SOLID", "Rotation_3_1"), model.selection("FACE", "Plane_1"), model.selection("FACE", "Plane_2")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)

Shell_1_objects = ["Partition_1_1_1/Modified_Face&Sketch_2/SketchLine_4",
                   "Partition_1_1_4/Modified_Face&Sketch_2/SketchLine_4",
                   "(Partition_1_1_2/Modified_Face&Revolution_1_1/To_Face)(Partition_1_1_2/Modified_Face&Sketch_2/SketchLine_1)"]
checkCylinderShell(Part_1_doc, Shell_1_objects, aLoc3, anAxis, 0.5 * ParamR.value(), ParamH.value())

# Test 4. Split cylinder and compose a solid
Partition_2 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Rotation_1_1"), model.selection("FACE", "PartSet/XOZ")])
Solid_1_objects = ["(Partition_2_1_1/Modified_Face&Cylinder_1_1/Face_3)(Partition_2_1_1/Modified_Face&PartSet/XOZ/XOZ)(Partition_2_1_1/Modified_Face&Cylinder_1_1/Face_2)",
                   "Partition_2_1_1/Modified_Face&Cylinder_1_1/Face_1&weak_name_2",
                   "Partition_2_1_1/Modified_Face&Cylinder_1_1/Face_2",
                   "Partition_2_1_1/Modified_Face&Cylinder_1_1/Face_3",
                   "Partition_2_1_2/Modified_Face&Cylinder_1_1/Face_1",
                   "Partition_2_1_2/Modified_Face&Cylinder_1_1/Face_2",
                   "Partition_2_1_2/Modified_Face&Cylinder_1_1/Face_3"]
checkCylinderSolid(Part_1_doc, Solid_1_objects, aLoc1, anAxis, 2 * ParamR.value(), ParamH.value())

# Test 5. Check non-cylinder
Sketch_3 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_2 = Sketch_3.addCircle(12.62721775445329, 9.188425784259302, 5)
SketchCircle_3 = Sketch_3.addCircle(16.49821418064359, 12.35313535520289, 5)
SketchConstraintRadius_2 = Sketch_3.setRadius(SketchCircle_2.results()[1], 5)
SketchConstraintEqual_1 = Sketch_3.setEqual(SketchCircle_2.results()[1], SketchCircle_3.results()[1])
SketchConstraintDistance_1 = Sketch_3.setDistance(SketchCircle_2.center(), SketchCircle_3.center(), 5, True)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_3")], model.selection(), 10, 0)
Solid_1_objects = [model.selection("FACE", "Extrusion_2_1_1/From_Face"), model.selection("FACE", "Extrusion_2_1_1/To_Face"), model.selection("FACE", "Extrusion_2_1_2/From_Face"), model.selection("FACE", "Extrusion_2_1_2/Generated_Face&Sketch_3/SketchCircle_3_2&weak_name_2"), model.selection("FACE", "Extrusion_2_1_2/Generated_Face&Sketch_3/SketchCircle_3_2&weak_name_1"), model.selection("FACE", "Extrusion_2_1_2/To_Face"), model.selection("FACE", "Extrusion_2_1_3/From_Face"), model.selection("FACE", "Extrusion_2_1_3/Generated_Face&Sketch_3/SketchCircle_2_2"), model.selection("FACE", "Extrusion_2_1_3/To_Face")]
Solid_1 = model.addSolid(Part_1_doc, Solid_1_objects)
checkNonCylinder(Solid_1)

# Test 6. Check non-cylindrical shell
Shell_1_objects = [model.selection("FACE", "(Partition_2_1_1/Modified_Face&Cylinder_1_1/Face_3)(Partition_2_1_1/Modified_Face&PartSet/XOZ/XOZ)(Partition_2_1_1/Modified_Face&Cylinder_1_1/Face_2)"),
                   model.selection("FACE", "Partition_2_1_1/Modified_Face&Cylinder_1_1/Face_1&weak_name_2"),
                   model.selection("FACE", "Partition_2_1_1/Modified_Face&Cylinder_1_1/Face_2"),
                   model.selection("FACE", "Partition_2_1_1/Modified_Face&Cylinder_1_1/Face_3"),
                   model.selection("FACE", "Partition_2_1_2/Modified_Face&Cylinder_1_1/Face_1"),
                   model.selection("FACE", "Partition_2_1_2/Modified_Face&Cylinder_1_1/Face_2"),
                   model.selection("FACE", "Partition_2_1_2/Modified_Face&Cylinder_1_1/Face_3")]
Shell_1 = model.addShell(Part_1_doc, Shell_1_objects)
checkNonCylindricalShell(Shell_1)

Shell_2 = model.addShell(Part_1_doc, [model.selection("FACE", "Extrusion_2_1_3/Generated_Face&Sketch_3/SketchCircle_2_2"), model.selection("FACE", "Extrusion_2_1_2/Generated_Face&Sketch_3/SketchCircle_3_2&weak_name_1")])
checkNonCylindricalShell(Shell_2)

model.end()
