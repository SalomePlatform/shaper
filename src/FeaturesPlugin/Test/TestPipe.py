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

# Get document
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

# Create a part
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

# Create a sketch with circle for pipe profile
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)

# Create circle
aSketchCircleFeature = aSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircleFeature.attribute("circle_center"))
aCircleRadius = aSketchCircleFeature.real("circle_radius")
anCircleCentr.setValue(0, 0)
aCircleRadius.setValue(10)
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
aFaceResult1 = aFaceFeature.firstResult()

# Create a sketch with edges for pipe path
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, -1, 0)

# Create line
aSketchLineFeature = aSketchFeature.addFeature("SketchLine")
aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
aSketchLineStartPoint.setValue(0, 0)
aSketchLineEndPoint.setValue(0, 200)

# Create arc
aSketchArcFeature = aSketchFeature.addFeature("SketchArc")
aSketchArcCenterPoint = geomDataAPI_Point2D(aSketchArcFeature.attribute("center_point"))
aSketchArcStartPoint = geomDataAPI_Point2D(aSketchArcFeature.attribute("start_point"))
aSketchArcEndPoint = geomDataAPI_Point2D(aSketchArcFeature.attribute("end_point"))
aSketchArcCenterPoint.setValue(100, 200)
aSketchArcStartPoint.setValue(200, 200)
aSketchArcEndPoint.setValue(0, 200)

aSession.finishOperation()
aSketchResult = aSketchFeature.firstResult()
aSketchShape = aSketchResult.shape()

# Create wire
aSession.startOperation()
aWireFeature = aPart.addFeature("Wire")
aBaseObjectsList = aWireFeature.selectionList("base_objects")
aShapeExplorer = GeomAPI_ShapeExplorer(aSketchShape, GeomAPI_Shape.EDGE)
while aShapeExplorer.more():
    aBaseObjectsList.append(aSketchResult, aShapeExplorer.current())
    aShapeExplorer.next()
aSession.finishOperation()
aWireResult = aWireFeature.firstResult()

# Create simple pipe
aSession.startOperation()
aPipeFeature = aPart.addFeature("Pipe")
aBaseObjectsList = aPipeFeature.selectionList("base_objects")
aBaseObjectsList.append(aFaceResult1, None)
aPathObjectSelection = aPipeFeature.selection("path_object")
aPathObjectSelection.setValue(aWireResult, None)
aPipeFeature.string("creation_method").setValue("simple")
aSession.finishOperation()

# Test results
assert (len(aPipeFeature.results()) > 0)
# aSession.undo()

# Create pipe with bi-normal
aSession.startOperation()
aPipeFeature = aPart.addFeature("Pipe")
aBaseObjectsList = aPipeFeature.selectionList("base_objects")
aBaseObjectsList.append(aFaceResult1, None)
aPathObjectSelection = aPipeFeature.selection("path_object")
aPathObjectSelection.setValue(aWireResult, None)
aPipeFeature.string("creation_method").setValue("binormal")
aBinormalObjectSelection = aPipeFeature.selection("binormal")
aShapeExplorer = GeomAPI_ShapeExplorer(aSketchShape, GeomAPI_Shape.EDGE)
aBinormalObjectSelection.setValue(aSketchResult, aShapeExplorer.current())
aSession.finishOperation()

# Test results
assert (len(aPipeFeature.results()) > 0)
# aSession.undo()

# Create pipe with locations
# Create a sketch with circle for pipe profile
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 200)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)

# Create circle
aSketchCircleFeature = aSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircleFeature.attribute("circle_center"))
aCircleRadius = aSketchCircleFeature.real("circle_radius")
anCircleCentr.setValue(0, 0)
aCircleRadius.setValue(20)
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
aFaceResult2 = aFaceFeature.firstResult()

aSession.startOperation()
aPipeFeature = aPart.addFeature("Pipe")
aBaseObjectsList = aPipeFeature.selectionList("base_objects")
aBaseObjectsList.append(aFaceResult1, None)
aBaseObjectsList.append(aFaceResult2, None)
aPathObjectSelection = aPipeFeature.selection("path_object")
aPathObjectSelection.setValue(aWireResult, None)
aPipeFeature.string("creation_method").setValue("locations")
aSession.finishOperation()

# Test results
assert (len(aPipeFeature.results()) > 0)

from salome.shaper import model
assert(model.checkPythonDump())
