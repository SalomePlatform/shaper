# Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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
      TestChamfer.py
      Unit test of FeaturesPlugin_Chamfer class
"""

from ModelAPI import *
from GeomDataAPI import *

__updated__ = "2017-11-30"

#=========================================================================
# Initialization of the test
#=========================================================================
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
# Create a part for extrusion
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
aPart = aSession.activeDocument()

# Create a box and a cylinder for filleting
aSession.startOperation()
aBox = aPart.addFeature("Box")
aBox.string("CreationMethod").setValue("BoxByDimensions")
aBox.real("dx").setValue(10)
aBox.real("dy").setValue(10)
aBox.real("dz").setValue(10)
aSession.finishOperation()

# a point to anchor a cylinder
aSession.startOperation()
aPoint = aPart.addFeature("Point")
aPoint.string("creation_method").setValue("by_xyz")
geomDataAPI_Point(aPoint.attribute("point3d")).setValue(20, 5, 0)
aSession.finishOperation()

aSession.startOperation()
aCylinder = aPart.addFeature("Cylinder")
aCylinder.string("CreationMethod").setValue("Cylinder")
aCylinder.selection("base_point").selectSubShape("vertex", "Point_1")
aCylinder.selection("axis").selectSubShape("edge", "PartSet/OZ")
aCylinder.real("radius").setValue(5)
aCylinder.real("height").setValue(10)
aSession.finishOperation()

#=========================================================================
# Test 1. Create chamfer with two distances
#=========================================================================
aSession.startOperation()
aChamfer1 = aPart.addFeature("Chamfer")
aChamfer1.string("creation_method").setValue("distance_distance")
anObjects = aChamfer1.selectionList("main_objects")
anObjects.append("[Box_1_1/Left][Box_1_1/Top]", "edge")
aChamfer1.real("d1").setValue(1.5)
aChamfer1.real("d2").setValue(2)
aSession.finishOperation()
assert(aChamfer1.error() == ""), "FAILED: Chamfer reports error \"{}\"".format(aChamfer1.error())

#=========================================================================
# Test 2. Change chamfer type
#=========================================================================
aSession.startOperation()
aChamfer1.string("creation_method").setValue("distance_angle")
aChamfer1.real("d").setValue(1)
aChamfer1.real("angle").setValue(30)
aSession.finishOperation()
assert(aChamfer1.error() == ""), "FAILED: Chamfer reports error \"{}\"".format(aChamfer1.error())

##=========================================================================
## Test 3. Check chamfer reports error if selected entities from different solids
##=========================================================================
aSession.startOperation()
aChamfer2 = aPart.addFeature("Chamfer")
aChamfer2.string("creation_method").setValue("distance_distance")
anObjects = aChamfer2.selectionList("main_objects")
anObjects.append("Box_1_1/Bottom", "face")
aChamfer2.real("d1").setValue(-2)
aChamfer2.real("d2").setValue(2)
aSession.finishOperation()
assert(aChamfer2.error() != ""), "FAILED: Chamfer does not report error"

##=========================================================================
## Test 4. Fix the error
##=========================================================================
aSession.startOperation()
aChamfer2.real("d1").setValue(1)
aSession.finishOperation()
assert(aChamfer2.error() == ""), "FAILED: Chamfer reports error \"{}\"".format(aChamfer2.error())

#=========================================================================
# Test 5. Check chamfer reports error on smoothly connected edges
#=========================================================================
aSession.startOperation()
aChamfer3 = aPart.addFeature("Chamfer")
aChamfer3.string("creation_method").setValue("distance_distance")
aChamfer3.selectionList("main_objects").append("Cylinder_1_1/Modified_Face_1", "face")
aChamfer3.real("d1").setValue(1)
aChamfer3.real("d2").setValue(1)
aSession.finishOperation()
assert(aChamfer3.lastResult() is None), "FAILED: Chamfer chamfer not produce a result"

#=========================================================================
# Test 6. Remove last fillet feature
#=========================================================================
aSession.startOperation()
aPart.removeFeature(aChamfer3)
aSession.finishOperation()

#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.checkPythonDump())
