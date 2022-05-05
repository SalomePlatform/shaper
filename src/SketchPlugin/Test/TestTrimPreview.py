# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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

from salome.shaper import model

from ModelAPI import *
from GeomDataAPI import *

from ConfigAPI import *
Config_PropManager().registerProp("Visualization", "operation_remove_feature_color", "Color of removed feature in operation", Config_Prop.Color, "255, 174, 201")

# base sketch
model.begin()
partSet = model.moduleDocument()
Part = model.addPart(partSet)
Part_doc = Part.document()
Sketch = model.addSketch(Part_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch.addLine(0, 20, 100, 20)
SketchLine_2 = Sketch.addLine(20, 0, 20, 40)
SketchLine_3 = Sketch.addLine(80, 0, 80, 40)
SketchCircle_1 = Sketch.addCircle(50, 50, 50)
model.do()
model.end()

aSession = ModelAPI_Session.get()
aSketchFeature = featureToCompositeFeature(Sketch.feature())

# Create Trim feature for a line
aSession.startOperation()
aTrim = aSketchFeature.addFeature("SketchTrim")
aSelectedObj = aTrim.reference("SelectedObject")
aSelectedObj.setValue(SketchLine_1.feature())
aSelectedPoint = geomDataAPI_Point2D(aTrim.attribute("SelectedPoint"))
aSelectedPoint.setValue(50, 20)
aPreviewObj = aTrim.reference("PreviewObject")
aPreviewObj.setValue(SketchLine_1.feature())
aPreviewPoint = geomDataAPI_Point2D(aTrim.attribute("PreviewPoint"))
aPreviewPoint.setValue(aSelectedPoint.pnt())
assert(featureToPresentation(aTrim).getAISObject(None) is not None)
aSession.finishOperation()

model.testNbSubFeatures(Sketch, "SketchArc", 0)
model.testNbSubFeatures(Sketch, "SketchCircle", 1)
model.testNbSubFeatures(Sketch, "SketchLine", 4)
model.testNbSubFeatures(Sketch, "SketchConstraintCoincidence", 2)

# Create Trim feature for a circle to perform its substitution by an arc
aSession.startOperation()
aTrim = aSketchFeature.addFeature("SketchTrim")
aSelectedObj = aTrim.reference("SelectedObject")
aSelectedObj.setValue(SketchCircle_1.feature())
aSelectedPoint = geomDataAPI_Point2D(aTrim.attribute("SelectedPoint"))
aSelectedPoint.setValue(50, 0)
aPreviewObj = aTrim.reference("PreviewObject")
aPreviewObj.setValue(SketchCircle_1.feature())
aPreviewPoint = geomDataAPI_Point2D(aTrim.attribute("PreviewPoint"))
aPreviewPoint.setValue(aSelectedPoint.pnt())
assert(featureToPresentation(aTrim).getAISObject(None) is not None)
aSession.finishOperation()

model.testNbSubFeatures(Sketch, "SketchArc", 1)
model.testNbSubFeatures(Sketch, "SketchCircle", 0)
model.testNbSubFeatures(Sketch, "SketchLine", 4)
model.testNbSubFeatures(Sketch, "SketchConstraintCoincidence", 4)
