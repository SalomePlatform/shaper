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
      TestFillet.py
      Unit test of FeaturesPlugin_Fillet class
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
# Test 1. Create fillet with fixed radius
#=========================================================================
aSession.startOperation()
aFillet1 = aPart.addFeature("Fillet")
aFillet1.string("creation_method").setValue("fixed_radius")
anObjects = aFillet1.selectionList("main_objects")
anObjects.append("[Box_1_1/Left][Box_1_1/Top]", "edge")
aFillet1.real("radius1").setValue(3)
aSession.finishOperation()
assert(aFillet1.error() == ""), "FAILED: Fillet reports error \"{}\"".format(aFillet1.error())

#=========================================================================
# Test 2. Change fillet type
#=========================================================================
aSession.startOperation()
aFillet1.string("creation_method").setValue("variable_radius")
aFillet1.real("radius1").setValue(5)
aFillet1.real("radius2").setValue(1)
aSession.finishOperation()
assert(aFillet1.error() == ""), "FAILED: Fillet reports error \"{}\"".format(aFillet1.error())

#=========================================================================
# Test 3. Check fillet reports error if selected entities from different solids
#=========================================================================
aSession.startOperation()
aFillet2 = aPart.addFeature("Fillet")
aFillet2.string("creation_method").setValue("fixed_radius")
anObjects = aFillet2.selectionList("main_objects")
anObjects.append("Cylinder_1_1/Face_2", "face")
anObjects.append("Box_1_1/Right", "face")
aFillet2.real("radius1").setValue(2)
aSession.finishOperation()
assert(aFillet2.error() != ""), "FAILED: Fillet does not report error"

#=========================================================================
# Test 4. Fix the error
#=========================================================================
aSession.startOperation()
anObjects.removeLast()
aSession.finishOperation()
assert(aFillet2.error() == ""), "FAILED: Fillet reports error \"{}\"".format(aFillet1.error())

#=========================================================================
# Test 5. Check fillet reports error on smoothly connected edges
#=========================================================================
aSession.startOperation()
aFillet3 = aPart.addFeature("Fillet")
aFillet3.string("creation_method").setValue("fixed_radius")
aFillet3.selectionList("main_objects").append("Cylinder_1_1/Modified_Face_1", "face")
aFillet3.real("radius1").setValue(3)
aSession.finishOperation()
assert(aFillet3.lastResult() is None), "FAILED: Fillet should not produce a result"

#=========================================================================
# Test 6. Remove last fillet feature
#=========================================================================
aSession.startOperation()
aPart.removeFeature(aFillet3)
aSession.finishOperation()

#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.checkPythonDump())
