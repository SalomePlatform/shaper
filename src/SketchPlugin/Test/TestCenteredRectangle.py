# Copyright (C) 2021-2022  CEA/DEN, EDF R&D
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

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
l = model.addParameter(Part_1_doc, "l", "50")
h = model.addParameter(Part_1_doc, "h", "20")

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(-25, -10, 25, -10)

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(-25, -10, 25, 10)
SketchLine_2.setAuxiliary(True)

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(25, -10, -25, 10)
SketchLine_3.setAuxiliary(True)

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(25, -10, 25, 10)

### Create SketchLine
SketchLine_5 = Sketch_1.addLine(25, 10, -25, 10)

### Create SketchLine
SketchLine_6 = Sketch_1.addLine(-25, 10, -25, -10)

### Create SketchPoint
SketchPoint_2 = Sketch_1.addPoint(0, 0)
SketchPoint_2.setAuxiliary(True)
Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_2.result())
Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_3.result())
Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_1.startPoint())
Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_1.startPoint())
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_4.startPoint())
Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchLine_4.startPoint())
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_5.startPoint())
Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_6.startPoint())
Sketch_1.setCoincident(SketchPoint_1.result(), SketchPoint_2.result())
Sketch_1.setHorizontal(SketchLine_1.result())
Sketch_1.setVertical(SketchLine_4.result())
Sketch_1.setHorizontal(SketchLine_5.result())
Sketch_1.setVertical(SketchLine_6.result())
Sketch_1.setLength(SketchLine_5.result(), "l")
Sketch_1.setLength(SketchLine_6.result(), "h")
model.do()

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1f-SketchLine_4f-SketchLine_5f-SketchLine_6f")], model.selection(), 40, 0, "Faces|Wires")

model.end()

#=============================================================================================
# Tests :
#=============================================================================================
from GeomDataAPI import *
from ModelAPI import *
from GeomAPI import *
from SketchAPI import SketchAPI_Sketch, SketchAPI_SketchEntity

def isSymetricX(line):
    aStart = geomDataAPI_Point2D(line.attribute("StartPoint"))
    aEnd   = geomDataAPI_Point2D(line.attribute("EndPoint"))
    return aStart.x() == -aEnd.x()

def isSymetricY(line):
    aStart = geomDataAPI_Point2D(line.attribute("StartPoint"))
    aEnd   = geomDataAPI_Point2D(line.attribute("EndPoint"))
    return aStart.y() == -aEnd.y()

def isSymetricXY(line):
    aStart = geomDataAPI_Point2D(line.attribute("StartPoint"))
    aEnd   = geomDataAPI_Point2D(line.attribute("EndPoint"))
    return (aStart.x() == -aEnd.x()) and (aStart.y() == -aEnd.y())

#=========================================================================
def checkLinesSymetry(aSketchFeature):
#=========================================================================
# Checks the lines of rectangle are symetric around the origin
	aNbSubs = aSketchFeature.numberOfSubs()
	aNbLines = 0
	nbAux = 0
	for i in range (0, aNbSubs):
		aFeature = objectToFeature(aSketchFeature.subFeature(i))
		if aFeature.getKind() == "SketchLine":
			aLastLine = aFeature
			entity = SketchAPI_SketchEntity(aLastLine)
			if entity.auxiliary().value(): # auxiliary line
				assert isSymetricXY(aLastLine)
				nbAux +=1
			else:
				assert (isSymetricX(aLastLine) or isSymetricY(aLastLine))
				aNbLines = aNbLines + 1
	assert (nbAux == 2)
	assert (aNbLines == 4)
	assert (model.dof(aSketchFeature) == 0)

Part_doc = model.activeDocument()
for feat in Part_doc.allFeatures():
	if feat.getKind() == "Sketch":
		sketch = feat

aSketchFeature = featureToCompositeFeature(sketch)
#=========================================================================
# Check the lines of rectangle are symetric around the origin
#=========================================================================
checkLinesSymetry(aSketchFeature)
#=========================================================================
# Change the dimensions of the rectangle
#=========================================================================
aSession = ModelAPI_Session.get()
aSession.startOperation()
l.setValue(80.)
h.setValue(55.)
aSession.finishOperation()
#=========================================================================
# Check the lines of rectangle are symetric around the origin
#=========================================================================
checkLinesSymetry(aSketchFeature)

assert(model.checkPythonDump())
