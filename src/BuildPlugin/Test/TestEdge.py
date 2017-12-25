## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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

# Initialization of the test
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

import random

def createLine(theSketchFeature):
    aSketchLineFeature = theSketchFeature.addFeature("SketchLine")
    aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
    aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
    aSketchLineStartPoint.setValue(random.uniform(0, 100), random.uniform(0, 100))
    aSketchLineEndPoint.setValue(random.uniform(0, 100), random.uniform(0, 100))

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
aNumOfLines = 10
for i in range(aNumOfLines):
    createLine(aSketchFeature)
aSession.finishOperation()
aSketchResult = aSketchFeature.firstResult()
aSketchShape = aSketchResult.shape()

# Create edges
aSession.startOperation()
anEdgeFeature = aPart.addFeature("Edge")
aBaseObjectsList = anEdgeFeature.selectionList("base_objects")
aShapeExplorer = GeomAPI_ShapeExplorer(aSketchShape, GeomAPI_Shape.EDGE)
while aShapeExplorer.more():
    aBaseObjectsList.append(aSketchResult, aShapeExplorer.current())
    aShapeExplorer.next()
aSession.finishOperation()

# Test results
assert (len(anEdgeFeature.results()) == aNumOfLines)

# Test edge building on edge of another result
aSession.startOperation()
aBox = aPart.addFeature("Box")
aBox.string("CreationMethod").setValue("BoxByDimensions")
aBox.real("dx").setValue(50)
aBox.real("dy").setValue(50)
aBox.real("dz").setValue(50)
aSession.finishOperation()
aBoxResult = aBox.firstResult()
aBoxShape = aBoxResult.shape()

# Create edges
aSession.startOperation()
anEdgeFeature2 = aPart.addFeature("Edge")
aBaseObjectsList = anEdgeFeature2.selectionList("base_objects")
aShapeExplorer = GeomAPI_ShapeExplorer(aBoxShape, GeomAPI_Shape.EDGE)
aShapes = []
while aShapeExplorer.more():
    # keep unique shapes only
    aCurrent = aShapeExplorer.current()
    isNewShape = True
    for s in aShapes:
        if s.isSame(aCurrent):
            isNewShape = False
            break
    if isNewShape:
        aShapes.append(aCurrent)
    aShapeExplorer.next()

for s in aShapes:
    aBaseObjectsList.append(aBoxResult, s)
aSession.finishOperation()

# Test results
assert (len(anEdgeFeature2.results()) == 12)

from salome.shaper import model
assert(model.checkPythonDump())
