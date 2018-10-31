## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from GeomAPI import *
from SketchAPI import *

from salome.shaper import model

import math

TOLERANCE = 1.e-7

def assertTorus(theTorus, theCenter, theAxis, theMajorRadius, theMinorRadius):
    assert(theTorus is not None)
    aCenter = theTorus.center()
    aDir = theTorus.direction()
    assert(aCenter.distance(theCenter) < TOLERANCE), "({}, {}, {}) != expected ({}, {}, {})".format(aCenter.x(), aCenter.y(), aCenter.z(), theCenter.x(), theCenter.y(), theCenter.z())
    assert(aDir.isParallel(theAxis, TOLERANCE)), "dir({}, {}, {}) is not parallel to dir({}, {}, {})".format(aDir.x(), aDir.y(), aDir.z(), theAxis.x(), theAxis.y(), theAxis.z())
    assert(math.fabs(theTorus.majorRadius() - theMajorRadius) < TOLERANCE), "Major radius {} != {}".format(theTorus.majorRadius(), theMajorRadius)
    assert(math.fabs(theTorus.minorRadius() - theMinorRadius) < TOLERANCE), "Minor radius {} != {}".format(theTorus.minorRadius(), theMinorRadius)

def checkTorusFace(theDocument, theFaceName, theCenter, theAxis, theMajorRadius, theMinorRadius):
    aFace = model.addFace(theDocument, [model.selection("FACE", theFaceName)])
    aShape = aFace.result().resultSubShapePair()[0].shape()
    assert(aShape.isFace())
    assertTorus(aShape.face().getTorus(), theCenter, theAxis, theMajorRadius, theMinorRadius)
    theDocument.removeFeature(aFace.feature())

def checkTorusShell(theDocument, theFaceNames, theCenter, theAxis, theMajorRadius, theMinorRadius):
    aSelection = []
    for name in theFaceNames:
        aSelection.append(model.selection("FACE", name))
    aShell = model.addShell(theDocument, aSelection)
    aShape = aShell.result().resultSubShapePair()[0].shape()
    assert(aShape.isShell())
    assertTorus(aShape.shell().getTorus(), theCenter, theAxis, theMajorRadius, theMinorRadius)
    theDocument.removeFeature(aShell.feature())

def checkTorusAll(theDocument, theFeature, theFaceName, theCenter, theAxis, theMajorRadius, theMinorRadius):
    aShape = theFeature.result().resultSubShapePair()[0].shape()
    assert(aShape.isSolid())
    assertTorus(aShape.solid().getTorus(), theCenter, theAxis, theMajorRadius, theMinorRadius)

    checkTorusShell(theDocument, [theFaceName], theCenter, theAxis, theMajorRadius, theMinorRadius)
    checkTorusFace(theDocument, theFaceName, theCenter, theAxis, theMajorRadius, theMinorRadius)


model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
ParamRMax = model.addParameter(Part_1_doc, "RMax", "15")
ParamRMin = model.addParameter(Part_1_doc, "RMin", "5")
ParamAngle = model.addParameter(Part_1_doc, "Angle", "30")
Torus_1 = model.addTorus(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "RMax", "RMin")
model.do()

# Test 1. Check torus
aCenter = GeomAPI.GeomAPI_Pnt(0, 0, 0)
anAxis = GeomAPI.GeomAPI_Dir(0, 0, 1)
checkTorusAll(Part_1_doc, Torus_1, "Torus_1_1/Face_1", aCenter, anAxis, ParamRMax.value(), ParamRMin.value())

# Test 2. Rotate torus
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Torus_1_1")], model.selection("EDGE", "PartSet/OX"), "Angle")
anAngle = ParamAngle.value() * math.pi / 180.0
aCosAngle = math.cos(anAngle)
aSinAngle = math.sin(anAngle)
aCenter = GeomAPI.GeomAPI_Pnt(0, aCenter.y() * aCosAngle - aCenter.z() * aSinAngle, aCenter.y() * aSinAngle + aCenter.z() * aCosAngle)
anAxis = GeomAPI.GeomAPI_Dir(0, anAxis.y() * aCosAngle - anAxis.z() * aSinAngle, anAxis.y() * aSinAngle + anAxis.z() * aCosAngle)
checkTorusAll(Part_1_doc, Rotation_1, "Rotation_1_1/MF:Rotated&Torus_1_1/Face_1", aCenter, anAxis, ParamRMax.value(), ParamRMin.value())

# Test 3. Split torus and compose a shell
Partition_1_objects = [model.selection("SOLID", "Rotation_1_1"), model.selection("FACE", "PartSet/YOZ"), model.selection("FACE", "PartSet/XOZ"), model.selection("FACE", "PartSet/XOY")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
Shell_1_objects = ["Partition_1_1_6/Modified_Face&Torus_1_1/Face_1",
                   "Partition_1_1_7/Modified_Face&Torus_1_1/Face_1&weak_name_2",
                   "Partition_1_1_7/Modified_Face&Torus_1_1/Face_1&weak_name_1",
                   "Partition_1_1_5/Modified_Face&Torus_1_1/Face_1"]
checkTorusShell(Part_1_doc, Shell_1_objects, aCenter, anAxis, ParamRMax.value(), ParamRMin.value())

model.end()
