# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

# Get document
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

# Create a part
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

# =============================================================================
# Test 1. Create face from edges of sketch
# =============================================================================

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

# Test results
assert (len(aFaceFeature.results()) > 0)

# =============================================================================
# Test 2. Create face from edges of solid
# =============================================================================

# Cylinder
aSession.startOperation()
aCylinder = aPart.addFeature("Cylinder")
aCylinder.string("CreationMethod").setValue("Cylinder")
aCylinder.selection("base_point").selectSubShape("VERTEX", "PartSet/Origin")
aCylinder.selection("axis").selectSubShape("EDGE", "PartSet/OZ")
aCylinder.real("radius").setValue(5)
aCylinder.real("height").setValue(10)
aSession.finishOperation()
aCylinderResult = aCylinder.firstResult()
aCylinderShape = aCylinderResult.shape()

# Create face
aSession.startOperation()
aFaceFeature2 = aPart.addFeature("Face")
aBaseObjectsList = aFaceFeature2.selectionList("base_objects")
aBaseObjectsList.append("[Cylinder_1_1/Face_1][Cylinder_1_1/Face_2]", "EDGE")
aSession.finishOperation()
assert (len(aFaceFeature2.results()) > 0)

# =============================================================================
# Test 3. Create face from face of solid
# =============================================================================
aSession.startOperation()
aCylinder = aPart.addFeature("Cylinder")
aCylinder.string("CreationMethod").setValue("Cylinder")
aCylinder.selection("base_point").selectSubShape("VERTEX", "PartSet/Origin")
aCylinder.selection("axis").selectSubShape("EDGE", "PartSet/OZ")
aCylinder.real("radius").setValue(5)
aCylinder.real("height").setValue(10)
aSession.finishOperation()
aCylinderResult = aCylinder.firstResult()
aCylinderShape = aCylinderResult.shape()

aSession.startOperation()
aFaceFeature3 = aPart.addFeature("Face")
aBaseObjectsList = aFaceFeature3.selectionList("base_objects")
aBaseObjectsList.append("Cylinder_2_1/Face_1", "FACE")
aSession.finishOperation()
assert (len(aFaceFeature3.results()) > 0)

# =============================================================================
# Test 4. Verify error is reported if selection of face feature is mixed (edges and face)
# =============================================================================

aSession.startOperation()
aCylinder = aPart.addFeature("Cylinder")
aCylinder.string("CreationMethod").setValue("Cylinder")
aCylinder.selection("base_point").selectSubShape("VERTEX", "PartSet/Origin")
aCylinder.selection("axis").selectSubShape("EDGE", "PartSet/OZ")
aCylinder.real("radius").setValue(5)
aCylinder.real("height").setValue(10)
aSession.finishOperation()
aCylinderResult = aCylinder.firstResult()
aCylinderShape = aCylinderResult.shape()

aSession.startOperation()
aFaceFeature4 = aPart.addFeature("Face")
aBaseObjectsList = aFaceFeature4.selectionList("base_objects")
aShapeExplorer = GeomAPI_ShapeExplorer(aSketchShape, GeomAPI_Shape.EDGE)
while aShapeExplorer.more():
    aBaseObjectsList.append(aSketchResult, aShapeExplorer.current())
    aShapeExplorer.next()
aBaseObjectsList.append("Cylinder_3_1/Face_3", "FACE")
aSession.finishOperation()
assert (len(aFaceFeature4.results()) == 0)
# remove failed feature
aSession.startOperation()
aPart.removeFeature(aFaceFeature4)
aSession.finishOperation()

# =============================================================================
# Test 5. Check Face feature failed on incorrect input
# =============================================================================

aSession.startOperation()
aFaceFeature5 = aPart.addFeature("Face")
aBaseObjectsList = aFaceFeature5.selectionList("base_objects")
aBaseObjectsList.append(aCylinderResult, None)
aSession.finishOperation()
assert (len(aFaceFeature5.results()) == 0)

aSession.startOperation()
aBaseObjectsList.clear()
aShapeExplorer = GeomAPI_ShapeExplorer(aCylinderShape, GeomAPI_Shape.VERTEX)
aShape = aShapeExplorer.current()
aBaseObjectsList.append(aCylinderResult, aShape)
aSession.finishOperation()
assert (len(aFaceFeature5.results()) == 0)

aSession.startOperation()
aBaseObjectsList.clear()
aShapeExplorer = GeomAPI_ShapeExplorer(aFaceFeature.lastResult().shape(), GeomAPI_Shape.EDGE)
aShape = aShapeExplorer.current()
aBaseObjectsList.append(aFaceFeature.lastResult(), aShape)
aSession.finishOperation()
assert (len(aFaceFeature5.results()) == 0)

# remove failed feature
aSession.startOperation()
aPart.removeFeature(aFaceFeature5)
aSession.finishOperation()

from salome.shaper import model
assert(model.checkPythonDump())
