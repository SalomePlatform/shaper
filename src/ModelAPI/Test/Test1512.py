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

"""
      Test1512.py
      Unit test for testing the correct selection of feature by the faces after the BOPs applied

"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

__updated__ = "2016-05-31"

aSession = ModelAPI_Session.get()
aPartSet = aSession.moduleDocument()

#=========================================================================
# Create a part
#=========================================================================
aSession.startOperation()
aPartFeature = aPartSet.addFeature("Part")
aSession.finishOperation()
assert (len(aPartFeature.results()) == 1)
aPart = aSession.activeDocument()

#=========================================================================
# Create a sketch rectangle in PartSet
#=========================================================================
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
aXOYPlane = objectToResult(aPartSet.objectByName("Construction", "XOY"))
aSketchFeature.selection("External").setValue(aXOYPlane, None)
# Create lines
aLine1 = aSketchFeature.addFeature("SketchLine")
geomDataAPI_Point2D(aLine1.attribute("StartPoint")).setValue(0, 0)
geomDataAPI_Point2D(aLine1.attribute("EndPoint")).setValue(200, 0)
aLine2 = aSketchFeature.addFeature("SketchLine")
geomDataAPI_Point2D(aLine2.attribute("StartPoint")).setValue(200, 0)
geomDataAPI_Point2D(aLine2.attribute("EndPoint")).setValue(200, 100)
aLine3 = aSketchFeature.addFeature("SketchLine")
geomDataAPI_Point2D(aLine3.attribute("StartPoint")).setValue(200, 100)
geomDataAPI_Point2D(aLine3.attribute("EndPoint")).setValue(0, 100)
aLine4 = aSketchFeature.addFeature("SketchLine")
geomDataAPI_Point2D(aLine4.attribute("StartPoint")).setValue(0, 100)
geomDataAPI_Point2D(aLine4.attribute("EndPoint")).setValue(0, 0)
aSession.finishOperation()

#=========================================================================
# Make extrusion on rectangle
#=========================================================================
aSession.startOperation()
aBox = aPart.addFeature("Extrusion")
aBox.selectionList("base").append(aSketchFeature.firstResult(), None)
aBox.string("CreationMethod").setValue("BySizes")
aBox.real("to_size").setValue(50)
aBox.real("from_size").setValue(0)
aSession.finishOperation()

#=========================================================================
# Create a cylindrical sketch for a big hole
#=========================================================================
aSession.startOperation()
aSketchFeature2 = featureToCompositeFeature(aPart.addFeature("Sketch"))
aSketchFeature2.selection("External").setValue(aXOYPlane, None);
aCirc1 = aSketchFeature2.addFeature("SketchCircle")
geomDataAPI_Point2D(aCirc1.attribute("circle_center")).setValue(100, 50)
aCirc1.real("circle_radius").setValue(40)
aSession.finishOperation()

#=========================================================================
# Make extrusion on a big circle
#=========================================================================
aSession.startOperation()
aHoleExt = aPart.addFeature("Extrusion")
aHoleExt.selectionList("base").append(aSketchFeature2.firstResult(), None)
aHoleExt.string("CreationMethod").setValue("BySizes")
aHoleExt.real("to_size").setValue(60)
aHoleExt.real("from_size").setValue(0)
aSession.finishOperation()

#=========================================================================
# Do cut a hole from a box
#=========================================================================
aSession.startOperation()
aCut1 = aPart.addFeature("Cut")
aCut1.selectionList("main_objects").append(aBox.firstResult(), None)
aCut1.selectionList("tool_objects").append(aHoleExt.firstResult(), None)
aSession.finishOperation()

#=========================================================================
# Create a cylindrical sketch for a small tower
#=========================================================================
aSession.startOperation()
aSketchFeature3 = featureToCompositeFeature(aPart.addFeature("Sketch"))
aSketchFeature3.selection("External").setValue(aXOYPlane, None);
aCirc2 = aSketchFeature3.addFeature("SketchCircle")
geomDataAPI_Point2D(aCirc2.attribute("circle_center")).setValue(20, 20)
aCirc2.real("circle_radius").setValue(10)
aSession.finishOperation()

#=========================================================================
# Make extrusion on a small circle
#=========================================================================
aSession.startOperation()
aTower = aPart.addFeature("Extrusion")
aTower.selectionList("base").append(aSketchFeature3.firstResult(), None)
aTower.string("CreationMethod").setValue("BySizes")
aTower.real("to_size").setValue(100)
aTower.real("from_size").setValue(0)
aSession.finishOperation()

#=========================================================================
# Do fuse with a tower. Tower must be an argument (not tool) to add the problem to the faces owners detection.
#=========================================================================
aSession.startOperation()
aFuse = aPart.addFeature("Fuse")
aFuse.string("creation_method").setValue("advanced")
aFuse.selectionList("main_objects").append(aTower.firstResult(), None)
aFuse.selectionList("tool_objects").append(aCut1.firstResult(), None)
aFuse.boolean("remove_intersection_edges").setValue(False)
aSession.finishOperation()

#=========================================================================
# Create a group feature to use the selection attribute for selection of faces by the name.
#=========================================================================
def check_owner(selection, topology_type, feature):
  aSession.startOperation()
  aGroup = aPart.addFeature("Group")
  aGroup.selectionList("group_list").append(selection, topology_type)
  aFace = aGroup.selectionList("group_list").value(0).value()
  aTestFeature = aPart.producedByFeature(aFuse.firstResult(), aFace)
  assert(aTestFeature.name() == feature.name())
  aSession.abortOperation()

# check faces
check_owner("Extrusion_1_1/Generated_Face_1", "face", aBox)
check_owner("Fuse_1_1/Modified_Face_2", "face", aBox)
check_owner("Cut_1_1/Modified_Face_1", "face", aHoleExt)
check_owner("Fuse_1_1/Modified_Face_1", "face", aTower)
# check edges without ambiguity
check_owner("Fuse_1_1/Modified_Face_2&Extrusion_1_1/Generated_Face_2", "edge", aBox)
check_owner("Fuse_1_1/Modified_Face_1&Extrusion_3_1/To_Face_1", "edge", aTower)

# check the connected topology method: solid is not a compound of connected topology
assert(aFuse.firstResult().shape().isConnectedTopology() == False)

#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.checkPythonDump())
