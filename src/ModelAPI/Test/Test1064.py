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

"""
      Test1064.py
      Unit test for testing the Part sub-shapes naming, described in the issue 1064

"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

__updated__ = "2015-10-16"

aSession = ModelAPI_Session.get()

#=========================================================================
# Create a sketch triangle in PartSet
#=========================================================================
aPartSet = aSession.moduleDocument()
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPartSet.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
# Create lines
aLine1 = aSketchFeature.addFeature("SketchLine")
geomDataAPI_Point2D(aLine1.attribute("StartPoint")).setValue(0, 0)
geomDataAPI_Point2D(aLine1.attribute("EndPoint")).setValue(200, 0)
aLine2 = aSketchFeature.addFeature("SketchLine")
geomDataAPI_Point2D(aLine2.attribute("StartPoint")).setValue(200, 0)
geomDataAPI_Point2D(aLine2.attribute("EndPoint")).setValue(0, 200)
aLine3 = aSketchFeature.addFeature("SketchLine")
geomDataAPI_Point2D(aLine3.attribute("StartPoint")).setValue(0, 200)
geomDataAPI_Point2D(aLine3.attribute("EndPoint")).setValue(0, 0)
aSession.finishOperation()

#=========================================================================
# Create a part
#=========================================================================
aSession.startOperation()
aPartFeature = aPartSet.addFeature("Part")
aSession.finishOperation()
assert (len(aPartFeature.results()) == 1)
aPart = aSession.activeDocument()

#=========================================================================
# Make extrusion on triangle
#=========================================================================
aSketchResult = modelAPI_ResultConstruction(aSketchFeature.firstResult())
aSession.startOperation()
anExtrusionFt = aPart.addFeature("Extrusion")
anExtrusionFt.selectionList("base").append(aSketchResult, aSketchResult.face(0))
anExtrusionFt.string("CreationMethod").setValue("BySizes")
anExtrusionFt.real("to_size").setValue(50)
anExtrusionFt.real("from_size").setValue(0)
aSession.finishOperation()


#=========================================================================
# Make a plane in PartSet on lateral face of the Extrusion
#=========================================================================
aSession.startOperation("Make Plane")
aSession.setActiveDocument(aPartSet)
aPlane = aPartSet.addFeature("Plane")
aPlane.string("creation_method").setValue("by_other_plane")
aPlane.string("by_other_plane_option").setValue("by_distance_from_other")
aPlane.selection("plane").selectSubShape("face", "Part_1/_weak_name_5_Extrusion_1_1")
aPlane.real("distance").setValue(0.001)
aPlane.boolean("reverse").setValue(False)
aSession.finishOperation()

#=========================================================================
# Update the sketch edges in order to update the plane on the lateral face automatically
#=========================================================================
aSession.startOperation("UpdateLine")
geomDataAPI_Point2D(aLine1.attribute("EndPoint")).setValue(400, 0)
geomDataAPI_Point2D(aLine2.attribute("StartPoint")).setValue(400, 0)
aSession.finishOperation()

#=========================================================================
# Check that the plane is also updated
#=========================================================================

assert(len(aPlane.results()) > 0)
aShape = aPlane.firstResult().shape()
aFace = GeomAPI_Face(aShape)
assert(aFace.isPlanar())
aPln = aFace.getPlane()
# Must be 0.4472135955, 0.894427191, 0.0
assert(aPln.direction().x() > 0.44)
assert(aPln.direction().x() < 0.45)
assert(aPln.direction().y() > 0.89)
assert(aPln.direction().y() < 0.90)
assert(aPln.direction().z() == 0.)

#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.checkPythonDump())
