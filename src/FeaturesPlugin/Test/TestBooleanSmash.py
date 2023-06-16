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

#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

aSession = ModelAPI_Session.get()
# Create a part for extrusions & boolean
aSession.startOperation()
aPartFeature = aSession.moduleDocument().addFeature("Part")
aSession.finishOperation()
aPart = aSession.activeDocument()
#=========================================================================
# Create a sketch with circle to extrude
#=========================================================================
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSketchCircle = aCircleSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("circle_center"))
aCircleRadius = aSketchCircle.real("circle_radius")
anCircleCentr.setValue(10., 10.)
aCircleRadius.setValue(50.)
aSession.finishOperation()
#=========================================================================
# Create a sketch with triangle to extrude
#=========================================================================
aSession.startOperation()
aTriangleSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aTriangleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aTriangleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aTriangleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSketchLineA = aTriangleSketchFeature.addFeature("SketchLine")
aSketchLineB = aTriangleSketchFeature.addFeature("SketchLine")
aSketchLineC = aTriangleSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aLineBStartPoint = geomDataAPI_Point2D(aSketchLineB.attribute("StartPoint"))
aLineBEndPoint = geomDataAPI_Point2D(aSketchLineB.attribute("EndPoint"))
aLineCStartPoint = geomDataAPI_Point2D(aSketchLineC.attribute("StartPoint"))
aLineCEndPoint = geomDataAPI_Point2D(aSketchLineC.attribute("EndPoint"))
aLineAStartPoint.setValue(25., 25.)
aLineAEndPoint.setValue(100., 25.)
aLineBStartPoint.setValue(100., 25.)
aLineBEndPoint.setValue(60., 75.)
aLineCStartPoint.setValue(60., 75.)
aLineCEndPoint.setValue(25., 25.)
aSession.finishOperation()
#=========================================================================
# Make extrusion on circle (cylinder) and triangle (prism)
#=========================================================================
# Build shape from sketcher results
aSession.startOperation()
extrudedObjects = []
for eachSketchFeature in [aCircleSketchFeature, aTriangleSketchFeature]:
    # Build sketch faces
    aSketchResult = modelAPI_ResultConstruction(eachSketchFeature.firstResult())
    # Create extrusion on them
    anExtrusionFt = aPart.addFeature("Extrusion")
    anExtrusionFt.selectionList("base").append(
        aSketchResult, aSketchResult.face(0))
    anExtrusionFt.string("CreationMethod").setValue("BySizes")
    anExtrusionFt.real("from_size").setValue(0)
    anExtrusionFt.real("to_size").setValue(50)
    anExtrusionFt.real("to_offset").setValue(0) #TODO: remove
    anExtrusionFt.real("from_offset").setValue(0) #TODO: remove
    anExtrusionFt.execute()
    extrudedObjects.append(modelAPI_ResultBody(anExtrusionFt.firstResult()))
aSession.finishOperation()
#=========================================================================
# Smash prism into the cylinder
#=========================================================================
aSession.startOperation()
aBooleanFt = aPart.addFeature("Smash")
aBooleanFt.selectionList("main_objects").append(extrudedObjects[0], extrudedObjects[0].shape())
aBooleanFt.selectionList("tool_objects").append(extrudedObjects[1], extrudedObjects[1].shape())
aBooleanFt.execute()
aSession.finishOperation()

assert (len(aBooleanFt.results()) > 0)
aBooleanResult = modelAPI_ResultBody(aBooleanFt.firstResult())
assert (aBooleanResult is not None)
#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.checkPythonDump())
