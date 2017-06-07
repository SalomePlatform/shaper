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

def createFace(thePart, theNormX, theNormY, theNormZ):
    # Create a sketch
    aSession.startOperation()
    aSketchFeature = featureToCompositeFeature(thePart.addFeature("Sketch"))
    anOrigin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
    anOrigin.setValue(0, 0, 0)
    aDirX = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
    aDirX.setValue(1, 0, 0)
    aNorm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
    aNorm.setValue(theNormX, theNormY, theNormZ)

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
    aFaceFeature = thePart.addFeature("Face")
    aBaseObjectsList = aFaceFeature.selectionList("base_objects")
    aShapeExplorer = GeomAPI_ShapeExplorer(aSketchShape, GeomAPI_Shape.EDGE)
    while aShapeExplorer.more():
        aBaseObjectsList.append(aSketchResult, aShapeExplorer.current())
        aShapeExplorer.next()
    aSession.finishOperation()
    return aFaceFeature.firstResult()

# Get document
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

# Create a part
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

# Create faces
aFaceResults = []
aFaceResults.append(createFace(aPart, 0, 0, 1))
aFaceResults.append(createFace(aPart, 0, 1, 0))

# Create shell
aSession.startOperation()
aShellFeature = aPart.addFeature("Shell")
aBaseObjectsList = aShellFeature.selectionList("base_objects")
for aFaceResult in aFaceResults:
    aBaseObjectsList.append(aFaceResult, None)
aSession.finishOperation()

# Test results
assert (len(aShellFeature.results()) > 0)

from salome.shaper import model
assert(model.checkPythonDump())
