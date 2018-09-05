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

def checkBox(theFeature, theCorner, theWidth, theDepth, theHeight):
    aShape = theFeature.result().resultSubShapePair()[0].shape()
    assert(aShape.isSolid())
    aBox = aShape.solid().getParallelepiped()
    assert(aBox is not None)
    assert(aBox.isAxesAligned())
    aCorner = aBox.corner()
    assert(aCorner.distance(theCorner) < TOLERANCE), "({}, {}, {}) != expected ({}, {}, {})".format(aCorner.x(), aCorner.y(), aCorner.z(), theCorner.x(), theCorner.y(), theCorner.z())
    assert(math.fabs(aBox.width() - theWidth) < TOLERANCE), "Width {} != {}".format(aBox.width(), theWidth)
    assert(math.fabs(aBox.depth() - theDepth) < TOLERANCE), "Depth {} != {}".format(aBox.depth(), theDepth)
    assert(math.fabs(aBox.height() - theHeight) < TOLERANCE), "Height {} != {}".format(aBox.height(), theHeight)

def assertParallelepiped(theBox, theCorner1, theCorner2):
    assert(theBox is not None)
    assert(theBox.isAxesAligned() == False)
    aCorner1 = theBox.corner()
    axes = theBox.axes()
    dirX = axes.dirX()
    dirY = axes.dirY()
    dirZ = axes.normal()
    aCorner2 = GeomAPI.GeomAPI_Pnt(aCorner1.x() + dirX.x() * theBox.width() + dirY.x() * theBox.depth() + dirZ.x() * theBox.height(),
                                   aCorner1.y() + dirX.y() * theBox.width() + dirY.y() * theBox.depth() + dirZ.y() * theBox.height(),
                                   aCorner1.z() + dirX.z() * theBox.width() + dirY.z() * theBox.depth() + dirZ.z() * theBox.height())
    assert(aCorner1.distance(theCorner1) < TOLERANCE), "({}, {}, {}) != expected ({}, {}, {})".format(aCorner1.x(), aCorner1.y(), aCorner1.z(), theCorner1.x(), theCorner1.y(), theCorner1.z())
    assert(aCorner2.distance(theCorner2) < TOLERANCE), "({}, {}, {}) != expected ({}, {}, {})".format(aCorner2.x(), aCorner2.y(), aCorner2.z(), theCorner2.x(), theCorner2.y(), theCorner2.z())

def checkRotatedBox(theFeature, theCorner1, theCorner2):
    aShape = theFeature.result().resultSubShapePair()[0].shape()
    assert(aShape.isSolid())
    aBox = aShape.solid().getParallelepiped()
    assertParallelepiped(aBox, theCorner1, theCorner2)

def checkShellRotatedBox(theDocument, theFaceNames, theCorner1, theCorner2):
    aSelection = []
    for name in theFaceNames:
        aSelection.append(model.selection("FACE", name))
    aShell = model.addShell(theDocument, aSelection)
    aShape = aShell.result().resultSubShapePair()[0].shape()
    assert(aShape.isShell())
    assertParallelepiped(aShape.shell().getParallelepiped(), theCorner1, theCorner2)
    theDocument.removeFeature(aShell.feature())

def checkShellNotBox(theDocument, theFaceNames):
    aSelection = []
    for name in theFaceNames:
        aSelection.append(model.selection("FACE", name))
    aShell = model.addShell(theDocument, aSelection)
    aShape = aShell.result().resultSubShapePair()[0].shape()
    assert(aShape.isShell())
    assert(aShape.shell().getParallelepiped() is None)
    theDocument.removeFeature(aShell.feature())


model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
ParamSize = model.addParameter(Part_1_doc, "BoxSize", "10")
ParamWidth = model.addParameter(Part_1_doc, "Width", "20")
ParamDepth = model.addParameter(Part_1_doc, "Depth", "10")
ParamHeight = model.addParameter(Part_1_doc, "Height", "25")
ParamAngle = model.addParameter(Part_1_doc, "Angle", "30")
Box_1 = model.addBox(Part_1_doc, "BoxSize", "BoxSize", "BoxSize")
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "Box_1_1/Front&Box_1_1/Right&Box_1_1/Top"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchLine_1 = Sketch_1.addLine(30, 10, 10, 10)
SketchLine_2 = Sketch_1.addLine(10, 10, 10, 20)
SketchLine_3 = Sketch_1.addLine(10, 20, 30, 20)
SketchLine_4 = Sketch_1.addLine(30, 20, 30, 10)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), "Width")
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), "Depth")
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2r-SketchLine_3r-SketchLine_4r")], model.selection(), "Height", 0)
model.do()

# Test 1. Check boxes
aCornerBox = GeomAPI.GeomAPI_Pnt(0, 0, 0)
checkBox(Box_1, aCornerBox, ParamSize.value(), ParamSize.value(), ParamSize.value())
aCornerPara = GeomAPI.GeomAPI_Pnt(ParamSize.value(), ParamSize.value(), ParamSize.value())
checkBox(Extrusion_1, aCornerPara, ParamWidth.value(), ParamDepth.value(), ParamHeight.value())

# Test 2. Rotate box to keep it still axes-aligned
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], model.selection("EDGE", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_1"), 90)
aCornerPara.setX(aCornerPara.x() + ParamWidth.value() - ParamDepth.value())
aCornerPara.setY(aCornerPara.y() - ParamWidth.value())
checkBox(Rotation_1, aCornerPara, ParamDepth.value(), ParamWidth.value(), ParamHeight.value())

# Test 3. Rotate boxes
Axis_4 = model.addAxis(Part_1_doc, model.selection("VERTEX", "Box_1_1/Back&Box_1_1/Left&Box_1_1/Bottom"), model.selection("VERTEX", "Box_1_1/Front&Box_1_1/Right&Box_1_1/Top"))
Rotation_2 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "Axis_1"), "Angle")
Rotation_3 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Rotation_1_1")], model.selection("EDGE", "Axis_1"), "Angle")

aRotDir = GeomAPI.GeomAPI_Dir(1, 1, 1)
anAngle = ParamAngle.value() * math.pi / 180.0
aCosAngle = math.cos(anAngle)
aSinAngle = math.sin(anAngle)

aCornerBox = GeomAPI.GeomAPI_Pnt(0, ParamSize.value(), 0)
aCornerBox1 = GeomAPI.GeomAPI_Pnt(ParamSize.value(), 0, ParamSize.value())
aCornerBox = GeomAPI.GeomAPI_Pnt(aCornerBox.x() * (aCosAngle + (1 - aCosAngle) * aRotDir.x()**2) + aCornerBox.y() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.y() - aSinAngle * aRotDir.z()) + aCornerBox.z() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.z() + aSinAngle * aRotDir.y()),
                                 aCornerBox.x() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.y() + aSinAngle * aRotDir.z()) + aCornerBox.y() * (aCosAngle + (1 - aCosAngle) * aRotDir.y()**2) + aCornerBox.z() * ((1 - aCosAngle) * aRotDir.y() * aRotDir.z() - aSinAngle * aRotDir.x()),
                                 aCornerBox.x() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.z() - aSinAngle * aRotDir.y()) + aCornerBox.y() * ((1 - aCosAngle) * aRotDir.y() * aRotDir.z() + aSinAngle * aRotDir.x()) + aCornerBox.z() * (aCosAngle + (1 - aCosAngle) * aRotDir.z()**2))
aCornerBox1 = GeomAPI.GeomAPI_Pnt(aCornerBox1.x() * (aCosAngle + (1 - aCosAngle) * aRotDir.x()**2) + aCornerBox1.y() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.y() - aSinAngle * aRotDir.z()) + aCornerBox1.z() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.z() + aSinAngle * aRotDir.y()),
                                  aCornerBox1.x() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.y() + aSinAngle * aRotDir.z()) + aCornerBox1.y() * (aCosAngle + (1 - aCosAngle) * aRotDir.y()**2) + aCornerBox1.z() * ((1 - aCosAngle) * aRotDir.y() * aRotDir.z() - aSinAngle * aRotDir.x()),
                                  aCornerBox1.x() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.z() - aSinAngle * aRotDir.y()) + aCornerBox1.y() * ((1 - aCosAngle) * aRotDir.y() * aRotDir.z() + aSinAngle * aRotDir.x()) + aCornerBox1.z() * (aCosAngle + (1 - aCosAngle) * aRotDir.z()**2))
checkRotatedBox(Rotation_2, aCornerBox, aCornerBox1)

aCornerPara.setY(aCornerPara.y() + ParamWidth.value())
aCornerPara1 = GeomAPI.GeomAPI_Pnt(aCornerPara.x() + ParamDepth.value(), aCornerPara.y() - ParamWidth.value(), aCornerPara.z() + ParamHeight.value())
aCornerPara = GeomAPI.GeomAPI_Pnt(aCornerPara.x() * (aCosAngle + (1 - aCosAngle) * aRotDir.x()**2) + aCornerPara.y() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.y() - aSinAngle * aRotDir.z()) + aCornerPara.z() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.z() + aSinAngle * aRotDir.y()),
                                  aCornerPara.x() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.y() + aSinAngle * aRotDir.z()) + aCornerPara.y() * (aCosAngle + (1 - aCosAngle) * aRotDir.y()**2) + aCornerPara.z() * ((1 - aCosAngle) * aRotDir.y() * aRotDir.z() - aSinAngle * aRotDir.x()),
                                  aCornerPara.x() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.z() - aSinAngle * aRotDir.y()) + aCornerPara.y() * ((1 - aCosAngle) * aRotDir.y() * aRotDir.z() + aSinAngle * aRotDir.x()) + aCornerPara.z() * (aCosAngle + (1 - aCosAngle) * aRotDir.z()**2))
aCornerPara1 = GeomAPI.GeomAPI_Pnt(aCornerPara1.x() * (aCosAngle + (1 - aCosAngle) * aRotDir.x()**2) + aCornerPara1.y() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.y() - aSinAngle * aRotDir.z()) + aCornerPara1.z() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.z() + aSinAngle * aRotDir.y()),
                                   aCornerPara1.x() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.y() + aSinAngle * aRotDir.z()) + aCornerPara1.y() * (aCosAngle + (1 - aCosAngle) * aRotDir.y()**2) + aCornerPara1.z() * ((1 - aCosAngle) * aRotDir.y() * aRotDir.z() - aSinAngle * aRotDir.x()),
                                   aCornerPara1.x() * ((1 - aCosAngle) * aRotDir.x() * aRotDir.z() - aSinAngle * aRotDir.y()) + aCornerPara1.y() * ((1 - aCosAngle) * aRotDir.y() * aRotDir.z() + aSinAngle * aRotDir.x()) + aCornerPara1.z() * (aCosAngle + (1 - aCosAngle) * aRotDir.z()**2))
checkRotatedBox(Rotation_3, aCornerPara, aCornerPara1)

# Test 4. Compose a non-closed shell of the box faces and check it is not a box
Shell_objects = ["Rotation_3_1/Rotated_Face_1", "Rotation_3_1/Rotated_Face_2", "Rotation_3_1/Rotated_Face_3", "Rotation_3_1/Rotated_Face_4"]
checkShellNotBox(Part_1_doc, Shell_objects)

# Test 5. Compose a shell of all box faces
Shell_objects = ["Rotation_3_1/Rotated_Face_1", "Rotation_3_1/Rotated_Face_2", "Rotation_3_1/Rotated_Face_3", "Rotation_3_1/Rotated_Face_4", "Rotation_3_1/Rotated_Face_5", "Rotation_3_1/Rotated_Face_6"]
checkShellRotatedBox(Part_1_doc, Shell_objects, aCornerPara, aCornerPara1)

model.end()
