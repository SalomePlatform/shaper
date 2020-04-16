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
# Test 1. Create wire from edges of sketch
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

# Create wire
aSession.startOperation()
aWireFeature = aPart.addFeature("Wire")
aBaseObjectsList = aWireFeature.selectionList("base_objects")
aShapeExplorer = GeomAPI_ShapeExplorer(aSketchShape, GeomAPI_Shape.EDGE)
if aShapeExplorer.more():
    aBaseObjectsList.append(aSketchResult, aShapeExplorer.current())
aWireFeature.customAction("add_contour")
aSession.finishOperation()

# Test results
assert (len(aWireFeature.results()) > 0)

from salome.shaper import model

# =============================================================================
# Test 2. Complete contour by selecting only one edge
# =============================================================================

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-68.61063464837, 232.418524871355, -488.85077186964, 232.418524871355)
SketchLine_2 = Sketch_1.addLine(-488.85077186964, 232.418524871355, -488.85077186964, -153.516295025729)
SketchLine_3 = Sketch_1.addLine(-488.85077186964, -153.516295025729, -68.61063464837, -153.516295025729)
SketchLine_4 = Sketch_1.addLine(-68.61063464837, -153.516295025729, -68.61063464837, 232.418524871355)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1")])
Wire_1.addContour()
model.end()

# =============================================================================
# Test 3. Create wire from edges of solid
# =============================================================================

# Box
aSession.startOperation()
aBox = aPart.addFeature("Box")
aBox.string("CreationMethod").setValue("BoxByDimensions")
aBox.real("dx").setValue(20)
aBox.real("dy").setValue(20)
aBox.real("dz").setValue(20)
aSession.finishOperation()
aBoxResult = aBox.firstResult()
aBoxShape = aBoxResult.shape()

# Wire
aSession.startOperation()
aWireFeature2 = aPart.addFeature("Wire")
aBaseObjectsList = aWireFeature2.selectionList("base_objects")
aBaseObjectsList.append("[Box_1_1/Front][Box_1_1/Bottom]", "EDGE")
aBaseObjectsList.append("[Box_1_1/Front][Box_1_1/Right]", "EDGE")
aBaseObjectsList.append("[Box_1_1/Right][Box_1_1/Top]", "EDGE")
aBaseObjectsList.append("[Box_1_1/Back][Box_1_1/Top]", "EDGE")
aBaseObjectsList.append("[Box_1_1/Back][Box_1_1/Left]", "EDGE")
aBaseObjectsList.append("[Box_1_1/Left][Box_1_1/Bottom]", "EDGE")
aSession.finishOperation()

# Test results
assert (len(aWireFeature2.results()) == 1)

# =============================================================================
# Test 4. Check Wire feature on the whole sketch
# =============================================================================

aSession.startOperation()
aWireFeature3 = aPart.addFeature("Wire")
aBaseObjectsList = aWireFeature3.selectionList("base_objects")
aBaseObjectsList.append(aSketchResult, None)
aSession.finishOperation()
assert (len(aWireFeature3.results()) == 1)

# =============================================================================
# Test 5. Check Wire feature failed on incorrect input
# =============================================================================

aSession.startOperation()
aWireFeature3 = aPart.addFeature("Wire")
aBaseObjectsList = aWireFeature3.selectionList("base_objects")
aShapeExplorer = GeomAPI_ShapeExplorer(aBoxShape, GeomAPI_Shape.VERTEX)
aShape = aShapeExplorer.current()
aBaseObjectsList.append(aBoxResult, aShape)
aSession.finishOperation()
assert (len(aWireFeature3.results()) == 0)

aSession.startOperation()
aBaseObjectsList.clear()
aShapeExplorer = GeomAPI_ShapeExplorer(aBoxShape, GeomAPI_Shape.FACE)
aShape = aShapeExplorer.current()
aBaseObjectsList.append(aBoxResult, aShape)
aSession.finishOperation()
assert (len(aWireFeature3.results()) == 0)

# remove failed feature
aSession.startOperation()
aPart.removeFeature(aWireFeature3)
aSession.finishOperation()

assert(model.checkPythonDump(model.ModelHighAPI.CHECK_NAMING))
