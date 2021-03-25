# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

# Initialization of the test
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

import random

def createPoint(theSketchFeature):
    aSketchPointFeature = theSketchFeature.addFeature("SketchPoint")
    aPointCoordinates = geomDataAPI_Point2D(aSketchPointFeature.attribute("PointCoordinates"))
    aPointCoordinates.setValue(random.uniform(0, 50), random.uniform(0, 50))
    return aSketchPointFeature

def createLine(theSketchFeature):
    aSketchLineFeature = theSketchFeature.addFeature("SketchLine")
    aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
    aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
    aSketchLineStartPoint.setValue(random.uniform(0, 50), random.uniform(0, 50))
    aSketchLineEndPoint.setValue(random.uniform(0, 50), random.uniform(0, 50))
    return aSketchLineFeature

# Get document
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

# Create a part
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

# Create a sketch
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
anOrigin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
anOrigin.setValue(0, 0, 0)
aDirX = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
aDirX.setValue(1, 0, 0)
aNorm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
aNorm.setValue(0, 0, 1)

# Create lines
aSketchLineFeature = aSketchFeature.addFeature("SketchLine")
aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
aSketchLineStartPoint.setValue(0, 0)
aSketchLineEndPoint.setValue(0, 50)
aSketchLineFeature = aSketchFeature.addFeature("SketchLine")
aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
aSketchLineStartPoint.setValue(0, 50)
aSketchLineEndPoint.setValue(50, 50)
aSketchLineFeature = aSketchFeature.addFeature("SketchLine")
aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
aSketchLineStartPoint.setValue(50, 50)
aSketchLineEndPoint.setValue(50, 0)
aSketchLineFeature = aSketchFeature.addFeature("SketchLine")
aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
aSketchLineStartPoint.setValue(50, 0)
aSketchLineEndPoint.setValue(0, 0)

aSession.finishOperation()
aSketchResult = aSketchFeature.firstResult()
aSketchShape = aSketchResult.shape()

# Create face
aSession.startOperation()
aFaceFeature = aPart.addFeature("Face")
aBaseObjectsList = aFaceFeature.selectionList("base_objects")
aShapeExplorer = GeomAPI_ShapeExplorer(aSketchShape, GeomAPI_Shape.EDGE)
while aShapeExplorer.more():
    aBaseObjectsList.append(aSketchResult, aShapeExplorer.current())
    aShapeExplorer.next()
aSession.finishOperation()
aFaceResult = aFaceFeature.firstResult()

# Create a sketch with points and lines
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
anOrigin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
anOrigin.setValue(0, 0, 0)
aDirX = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
aDirX.setValue(1, 0, 0)
aNorm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
aNorm.setValue(0, 0, 1)

# Create points
aNumOfPoints = 10
aPoints = []
for i in range(aNumOfPoints):
    aSession.startOperation()
    aSketchPointFeature = createPoint(aSketchFeature)
    aSession.finishOperation()
    aPoints.append(aSketchPointFeature.firstResult().shape())

# Create lines
aNumOfLines = 10
aLines = []
for i in range(aNumOfLines):
    aSession.startOperation()
    aSketchLineFeature = createLine(aSketchFeature)
    aSession.finishOperation()
    aLines.append(aSketchLineFeature.firstResult().shape())

aSession.finishOperation()
aSketchResult = aSketchFeature.firstResult()
aSketchShape = aSketchResult.shape()

# Create sub-shapes
aSession.startOperation()
aSubShapesFeature = aPart.addFeature("SubShapes")
aBaseShapeSelection = aSubShapesFeature.selection("base_shape")
aBaseShapeSelection.setValue(aFaceResult, None)
aSubShapesList = aSubShapesFeature.selectionList("subshapes")
for aPoint in aPoints:
    aSubShapesList.append(aSketchResult, aPoint)
aShapeExplorer = GeomAPI_ShapeExplorer(aSketchShape, GeomAPI_Shape.EDGE)
while aShapeExplorer.more():
    aSubShapesList.append(aSketchResult, aShapeExplorer.current())
    aShapeExplorer.next()
aSession.finishOperation()

# Test results
assert (len(aSubShapesFeature.results()) > 0)

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-454.545454545455, 50.600343053173, 137.95899463189)
SketchCircle_2 = Sketch_1.addCircle(-454.545454545455, 50.600343053173, 62.129572131303)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchCircle_2.center())
SketchPoint_1 = Sketch_1.addPoint(-490.566037735849, 50.600343053173)
SketchPoint_2 = Sketch_1.addPoint(-423.670668953688, 50.600343053173)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchCircle_1_2")])
Face_2 = model.addFace(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchCircle_2_2")])
SubShapes_1 = model.addSubShapes(Part_1_doc, model.selection("FACE", "Face_1_1"), [model.selection("VERTEX", "Sketch_1/SketchPoint_1"), model.selection("VERTEX", "Sketch_1/SketchPoint_2")])
SubShapes_1.setBaseShape(model.selection("FACE", "Face_2_1"))
model.end()

assert(model.checkPythonDump())
