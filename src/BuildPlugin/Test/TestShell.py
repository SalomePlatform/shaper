# Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

# Test shell building on set of faces from another result

# Cylinder
aSession.startOperation()
aCylinder = aPart.addFeature("Cylinder")
aCylinder.string("CreationMethod").setValue("Cylinder")
aCylinder.selection("base_point").selectSubShape("VERTEX", "PartSet/Origin")
aCylinder.selection("axis").selectSubShape("EDGE", "PartSet/OZ")
aCylinder.real("radius").setValue(25)
aCylinder.real("height").setValue(50)
aSession.finishOperation()
aCylinderResult = aCylinder.firstResult()
aCylinderShape = aCylinderResult.shape()

# Create shell
aSession.startOperation()
aShellFeature2 = aPart.addFeature("Shell")
aBaseObjectsList = aShellFeature2.selectionList("base_objects")
aBaseObjectsList.append("Cylinder_1_1/Face_1", "FACE")
aBaseObjectsList.append("Cylinder_1_1/Face_3", "FACE")
aSession.finishOperation()

# Test results
assert (len(aShellFeature2.results()) > 0)

# Check Shell feature failed on incorrect input
aShellResult = aShellFeature.firstResult()
aSession.startOperation()
aShellFeature3 = aPart.addFeature("Shell")
aBaseObjectsList = aShellFeature3.selectionList("base_objects")
aShapeExplorer = GeomAPI_ShapeExplorer(aShellResult.shape(), GeomAPI_Shape.EDGE)
while aShapeExplorer.more():
    aBaseObjectsList.append(aShellResult, aShapeExplorer.current())
    aShapeExplorer.next()
aSession.finishOperation()
assert (len(aShellFeature3.results()) == 0)

# remove failed feature
aSession.startOperation()
aPart.removeFeature(aShellFeature3)
aSession.finishOperation()

from salome.shaper import model
assert(model.checkPythonDump())
