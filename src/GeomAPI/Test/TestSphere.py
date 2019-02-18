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

from GeomAPI import *
from SketchAPI import *

from salome.shaper import model

import math

TOLERANCE = 1.e-7

def checkArc(theDocument, theEdgeName, theCenter, theRadius):
    # check edge (arc of circle)
    anEdge = model.addEdge(theDocument, [model.selection("EDGE", theEdgeName)])
    aShape = anEdge.result().resultSubShapePair()[0].shape()
    assert(aShape.isEdge())
    anArcEdge = aShape.edge()
    assert(anArcEdge.isArc())
    aCircle = anArcEdge.circle()
    assert(aCircle.center().distance(theCenter) < TOLERANCE)
    assert(math.fabs(aCircle.radius() - theRadius) < TOLERANCE)
    theDocument.removeFeature(anEdge.feature())

def assertSphere(theSphere, theCenter, theRadius):
    assert(theSphere is not None)
    assert(theSphere.center().distance(theCenter) < TOLERANCE)
    assert(math.fabs(theSphere.radius() - theRadius) < TOLERANCE)

def checkSphereFace(theDocument, theFaceName, theCenter, theRadius):
    # check spherical face
    aFace = model.addFace(theDocument, [model.selection("FACE", theFaceName)])
    aShape = aFace.result().resultSubShapePair()[0].shape()
    assert(aShape.isFace())
    assertSphere(aShape.face().getSphere(), theCenter, theRadius)
    theDocument.removeFeature(aFace.feature())

def checkSphereShell(theDocument, theFaceName, theCenter, theRadius):
    # check spherical shell
    aShell = model.addShell(theDocument, [model.selection("FACE", theFaceName)])
    aShape = aShell.result().resultSubShapePair()[0].shape()
    assert(aShape.isShell())
    assertSphere(aShape.shell().getSphere(), theCenter, theRadius)
    theDocument.removeFeature(aShell.feature())

def checkSphereAll(theDocument, theFeature, theFaceName, theCenter, theRadius):
    # check solid
    aShape = theFeature.result().resultSubShapePair()[0].shape()
    assert(aShape.isSolid())
    assertSphere(aShape.solid().getSphere(), theCenter, theRadius)

    checkSphereShell(theDocument, theFaceName, theCenter, theRadius)
    checkSphereFace(theDocument, theFaceName, theCenter, theRadius)

    anArcName = "[" + theFaceName + "][weak_name_3]"
    checkArc(theDocument, anArcName, theCenter, theRadius)

def checkNonSphereShell(theFeature):
    aShape = theFeature.result().resultSubShapePair()[0].shape()
    assert(aShape.isShell())
    assert(aShape.shell().getSphere() is None)


model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
ParamR = model.addParameter(Part_1_doc, "R", "50")
ParamShift = model.addParameter(Part_1_doc, "Shift", "30")
ParamAngle = model.addParameter(Part_1_doc, "Angle", "30")
ParamAperture = model.addParameter(Part_1_doc, "Aperture", "360")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchArc_1 = Sketch_1.addArc(0, 0, 0, -50, 0, 50, False)
SketchLine_1 = Sketch_1.addLine(0, 50, 0, -50)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_1.endPoint())
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchArc_1.center())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], "R")
model.do()

# Test 1. Compose sphere
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_1_2r-SketchLine_1f")], model.selection("EDGE", "Sketch_1/SketchLine_1"), "Aperture", 0)
aCenter = GeomAPI.GeomAPI_Pnt(0, 0, 0)
checkSphereAll(Part_1_doc, Revolution_1, "Revolution_1_1/Generated_Face&Sketch_1/SketchArc_1_2", aCenter, ParamR.value())

# Test 2. Translate sphere
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Revolution_1_1")], model.selection("EDGE", "PartSet/OX"), "Shift")
aCenter.setX(aCenter.x() + ParamShift.value())
checkSphereAll(Part_1_doc, Translation_1, "Translation_1_1/MF:Translated&Sketch_1/SketchArc_1_2", aCenter, ParamR.value())

# Test 3. Rotate sphere
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Translation_1_1")], model.selection("EDGE", "PartSet/OY"), "Angle")
anAngle = ParamAngle.value() * math.pi / 180.0
aCenter.setX(ParamShift.value() * math.cos(anAngle))
aCenter.setZ(-ParamShift.value() * math.sin(anAngle))
checkSphereAll(Part_1_doc, Rotation_1, "Rotation_1_1/MF:Rotated&Sketch_1/SketchArc_1_2", aCenter, ParamR.value())

# Test 4. Check result by changing parameters
ParamR.setValue(100)
model.do()
checkSphereAll(Part_1_doc, Rotation_1, "Rotation_1_1/MF:Rotated&Sketch_1/SketchArc_1_2", aCenter, ParamR.value())

ParamAperture.setValue(270)
model.do()
checkSphereFace(Part_1_doc, "Rotation_1_1/MF:Rotated&Sketch_1/SketchArc_1_2", aCenter, ParamR.value())
checkSphereShell(Part_1_doc, "Rotation_1_1/MF:Rotated&Sketch_1/SketchArc_1_2", aCenter, ParamR.value())
checkArc(Part_1_doc, "[Rotation_1_1/MF:Rotated&Sketch_1/SketchArc_1_2][Rotation_1_1/MF:Rotated&Revolution_1_1/From_Face]", aCenter, ParamR.value())
checkArc(Part_1_doc, "[Rotation_1_1/MF:Rotated&Sketch_1/SketchArc_1_2][Rotation_1_1/MF:Rotated&Revolution_1_1/To_Face]", aCenter, ParamR.value())

# Test 5. Sheck non-spherical shell
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Rotation_1_1/MF:Rotated&Revolution_1_1/To_Face"), model.selection("FACE", "Rotation_1_1/MF:Rotated&Revolution_1_1/From_Face")])
checkNonSphereShell(Shell_1)

Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_2 = Sketch_2.addLine(18.12152721893265, 20.53645178481853, 73.15172297255518, 20.53645178481853)
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_2.result())
SketchArc_2 = Sketch_2.addArc(60.28852631862447, 20.53645178481853, 73.15172297255518, 20.53645178481853, 58.16589238004093, 33.22330534748203, False)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_2.result(), SketchArc_2.center())
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchArc_2.startPoint())
SketchArc_3 = Sketch_2.addArc(40.15343392262997, 20.53645178481853, 18.12152721893265, 20.53645178481853, 58.16589238004093, 33.22330534748203, True)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_2.result(), SketchArc_3.center())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_2.startPoint(), SketchArc_3.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchArc_2.endPoint(), SketchArc_3.endPoint())
model.do()
Revolution_2 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_2r-SketchArc_2_2f-SketchArc_3_2f")], model.selection("EDGE", "Sketch_2/SketchLine_2"), 360, 0)
Shell_2 = model.addShell(Part_1_doc, [model.selection("FACE", "Revolution_2_1/Generated_Face&Sketch_2/SketchArc_3_2"), model.selection("FACE", "Revolution_2_1/Generated_Face&Sketch_2/SketchArc_2_2")])
checkNonSphereShell(Shell_2)

model.end()
