# Copyright (C) 2014-2023  CEA, EDF
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
import math
from ModelAPI import *

from salome.shaper import model

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

# Create a part
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
assert (len(aPartFeature.results()) == 1)
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

#=========================================================================
# Create a first point
#=========================================================================
aSession.startOperation()
aPointFeature = aPart.addFeature("Point")
aPointFeatureData = aPointFeature.data()
assert(aPointFeatureData is not None)
geomDataAPI_Point(aPointFeatureData.attribute("point3d")).setValue(0., 0., 0.)
aPointFeatureData.string("creation_method").setValue("by_xyz")
aPointFeature.execute()
aSession.finishOperation()
aPoint1Result = aPointFeature.firstResult();

#=========================================================================
# Create a second point
#=========================================================================
aSession.startOperation()
aPointFeature = aPart.addFeature("Point")
aPointFeatureData = aPointFeature.data()
assert(aPointFeatureData is not None)
geomDataAPI_Point(aPointFeatureData.attribute("point3d")).setValue(0., 0., 100.)
aPointFeatureData.string("creation_method").setValue("by_xyz")
aPointFeature.execute()
aSession.finishOperation()
aPoint2Result = aPointFeature.firstResult();

#=========================================================================
# Create axis by two points
#=========================================================================
aSession.startOperation()
anAxisFeature = aPart.addFeature("Axis")
anAxisFeatureData = anAxisFeature.data()
assert(anAxisFeatureData is not None)
anAxisFeatureData.string("CreationMethod").setValue("AxisByPointsCase")
anAxisFeatureData.selection("FirstPoint").setValue(aPoint1Result, aPoint1Result.shape())
anAxisFeatureData.selection("SecondPoint").setValue(aPoint2Result, aPoint2Result.shape())
anAxisFeature.execute()
aSession.finishOperation()

assert (len(anAxisFeature.results()) > 0)
anAxisResult = modelAPI_ResultConstruction(anAxisFeature.firstResult())
assert (anAxisResult is not None)

#=========================================================================
# Create a sketch circle to extrude
#=========================================================================
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
# Create circle
aSketchCircle = aCircleSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("circle_center"))
aCircleRadius = aSketchCircle.real("circle_radius")
anCircleCentr.setValue(0, 0)
aCircleRadius.setValue(50)
aSession.finishOperation()

#=========================================================================
# Make extrusion on circle
#=========================================================================
# Build shape from sketcher results
aCircleSketchResult = modelAPI_ResultConstruction(aCircleSketchFeature.firstResult())
assert (aCircleSketchResult.facesNum() > 0)

# Create extrusion
aSession.startOperation()
anExtrusionFt = aPart.addFeature("Extrusion")
assert (anExtrusionFt.getKind() == "Extrusion")
# selection type FACE=4
anExtrusionFt.selectionList("base").append(aCircleSketchResult, aCircleSketchResult.face(0))
anExtrusionFt.string("CreationMethod").setValue("BySizes")
anExtrusionFt.real("to_size").setValue(100)
anExtrusionFt.real("from_size").setValue(0)
anExtrusionFt.real("to_offset").setValue(0) #TODO: remove
anExtrusionFt.real("from_offset").setValue(0) #TODO: remove
anExtrusionFt.execute()
aSession.finishOperation()
assert (anExtrusionFt.real("to_size").value() == 100.0)

# Check extrusion results
assert (len(anExtrusionFt.results()) > 0)
anExtrusionResult = modelAPI_ResultBody(anExtrusionFt.firstResult())
assert (anExtrusionResult is not None)

#=========================================================================
# Create axis by cylinder
#=========================================================================
aSession.startOperation()
anAxisFeature = aPart.addFeature("Axis")
anAxisFeatureData = anAxisFeature.data()
assert(anAxisFeatureData is not None)
anAxisFeatureData.string("CreationMethod").setValue("AxisByCylindricalFaceCase")
anAxisFeatureData.selection("CylindricalFace").selectSubShape("face", "Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2")
anAxisFeature.execute()
aSession.finishOperation()

assert (len(anAxisFeature.results()) > 0)
anAxisResult = modelAPI_ResultConstruction(anAxisFeature.firstResult())
assert (anAxisResult is not None)

# Create a sketch with line
aSession.startOperation()
anOrigin = GeomAPI_Pnt(0, 0, 0)
aDirX = GeomAPI_Dir(1, 0, 0)
aNorm = GeomAPI_Dir(0, 0, 1)
aSketch = model.addSketch(aPart, GeomAPI_Ax3(anOrigin, aDirX, aNorm))
aSketchLine = aSketch.addLine(0, 0, 100, 100)
aSession.finishOperation()

# Test an axis by line
aSession.startOperation()
anAxis = model.addAxis(aPart, aSketchLine.result())
aSession.finishOperation()
assert (len(anAxis.results()) > 0)

# Create plane
aSession.startOperation()
aPlane1 = model.addPlane(aPart, 1, 1, 1, 0)
aSession.finishOperation()

# Create a sketch with point
aSession.startOperation()
anOrigin = GeomAPI_Pnt(0, 0, 0)
aDirX = GeomAPI_Dir(1, 0, 0)
aNorm = GeomAPI_Dir(0, 0, 1)
aSketch = model.addSketch(aPart, GeomAPI_Ax3(anOrigin, aDirX, aNorm))
aSketchPoint = aSketch.addPoint(50, 50)
aSession.finishOperation()

# Test an axis by plane and point
aSession.startOperation()
anAxis = model.addAxis(aPart, aPlane1.result(), aSketchPoint.result())
aSession.finishOperation()
assert (len(anAxis.results()) > 0)

# Create plane
aSession.startOperation()
aPlane2 = model.addPlane(aPart, 0, 1, 1, 0)
aSession.finishOperation()

# Test an axis by two planes
aSession.startOperation()
anAxis = model.addAxis(aPart, aPlane1.result(), aPlane2.result())
aSession.finishOperation()
assert (len(anAxis.results()) > 0)

# Test an axis by two planes and offsets
aSession.startOperation()
anAxis = model.addAxis(aPart, aPlane1.result(), 50, False, aPlane2.result(), 100, True)
aSession.finishOperation()
assert (len(anAxis.results()) > 0)

assert(model.checkPythonDump())
